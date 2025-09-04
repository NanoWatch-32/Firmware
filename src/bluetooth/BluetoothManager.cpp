#include "BluetoothManager.h"
#include <ArduinoJson.h>
#include <memory>

BluetoothManager bluetooth_manager;

BluetoothManager::BluetoothManager() = default;

void BluetoothManager::begin(const std::string &deviceName) {
    NimBLEDevice::init(deviceName);
    pServer = NimBLEDevice::createServer();
    customService = pServer->createService("0b60ab11-bc40-4d00-9ea4-5f2406872d9f");
    commandChar = customService->createCharacteristic(
        "cfa93afb-2c3d-4a76-a182-67e8b6d50b55",
        WRITE | NOTIFY
    );

    commandCallbacks = new CommandCallbacks(this);
    commandChar->setCallbacks(commandCallbacks);
    customService->start();

    serverCallbacks = new ServerCallbacks(this);
    pServer->setCallbacks(serverCallbacks);

    pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName(deviceName);
    pAdvertising->setAppearance(0x00C0);
    pAdvertising->addServiceUUID(customService->getUUID());
    pAdvertising->start();
}

void BluetoothManager::setPin(const std::string &pin) {
    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    NimBLEDevice::setSecurityPasskey(std::stoi(pin));
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
}

void BluetoothManager::startAdvertising() {
    if (pAdvertising) pAdvertising->start();
}

void BluetoothManager::stopAdvertising() {
    if (pAdvertising) pAdvertising->stop();
}

void BluetoothManager::onConnect(std::function<void(NimBLEServer *)> callback) {
    connectCallback = callback;
}

void BluetoothManager::onDisconnect(std::function<void(NimBLEServer *)> callback) {
    disconnectCallback = callback;
}

bool BluetoothManager::isConnected() const {
    return pServer && pServer->getConnectedCount() > 0;
}

bool BluetoothManager::isAdvertising() const {
    return pAdvertising && pAdvertising->isAdvertising();
}

void BluetoothManager::send(const Packet &packet) const {
    if (!isConnected()) {
        Serial.println("Cannot send: Not connected.");
        return;
    }

    // Serialize packet to JSON (without type)
    StaticJsonDocument<64> doc;
    packet.serialize(doc);

    String output;
    serializeJson(doc, output);

    const int maxChunkSize = 18; // 20 bytes MTU - 2 bytes for header
    int totalLength = output.length();
    int numFragments = (totalLength + maxChunkSize - 1) / maxChunkSize;

    uint8_t header[2];
    header[0] = static_cast<uint8_t>(packet.getType());

    for (int i = 0; i < numFragments; i++) {
        header[1] = (i << 4) | numFragments;

        String fragment;
        fragment += (char) header[0];
        fragment += (char) header[1];

        int start = i * maxChunkSize;
        int end = min(start + maxChunkSize, totalLength);
        fragment += output.substring(start, end);

        commandChar->setValue(fragment.c_str());
        commandChar->notify();
        delay(10);
    }

    Serial.println("Sent packet.");
}

void BluetoothManager::listen(PacketType type, std::function<void(const Packet &)> callback) {
    listeners[type] = callback;
}


void BluetoothManager::ServerCallbacks::onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) {
    if (manager->connectCallback) manager->connectCallback(pServer);
}

void BluetoothManager::ServerCallbacks::onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) {
    if (manager->disconnectCallback) manager->disconnectCallback(pServer);
    manager->startAdvertising();
}


void BluetoothManager::CommandCallbacks::onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) {
    std::string value = pCharacteristic->getValue();
    if (value.length() < 2) return;

    uint8_t packetTypeByte = value[0];
    uint8_t fragHeader = value[1];
    uint8_t fragmentIndex = (fragHeader >> 4) & 0x0F;
    uint8_t totalFragments = fragHeader & 0x0F;

    std::string payload = value.substr(2);

    if (fragmentIndex == 0) {
        manager->currentPacketType = static_cast<PacketType>(packetTypeByte);
        manager->fragmentBuffer = payload.c_str();
        manager->expectedFragments = totalFragments;
        manager->receivedFragments = 1;
    } else {
        if (fragmentIndex == manager->receivedFragments) {
            manager->fragmentBuffer += payload.c_str();
            manager->receivedFragments++;
        } else {
            manager->fragmentBuffer = "";
            manager->expectedFragments = 0;
            manager->receivedFragments = 0;
            manager->currentPacketType = static_cast<PacketType>(0);
            return;
        }
    }

    if (manager->receivedFragments == manager->expectedFragments) {
        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, manager->fragmentBuffer);

        if (error) {
            Serial.print("JSON deserialization failed: ");
            Serial.println(error.c_str());
            return;
        }

        std::unique_ptr<Packet> packet(createPacketFromType(manager->currentPacketType));
        if (!packet) {
            Serial.print("Unknown packet type: ");
            Serial.println(static_cast<uint8_t>(manager->currentPacketType));
            return;
        }

        packet->deserialize(doc);

        Serial.print("Received packet with type: ");
        Serial.println(static_cast<uint8_t>(packet->getType()));

        auto it = manager->listeners.find(packet->getType());
        if (it != manager->listeners.end()) {
            it->second(*packet);
        } else {
            Serial.println("No listener registered for this packet type");
        }

        manager->fragmentBuffer = "";
        manager->expectedFragments = 0;
        manager->receivedFragments = 0;
        manager->currentPacketType = static_cast<PacketType>(0);
    }
}
