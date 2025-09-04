#include "BluetoothManager.h"
#include <vector>
#include <memory>

BluetoothManager bluetooth_manager;

BluetoothManager::BluetoothManager() = default;

void BluetoothManager::begin(const std::string &deviceName) {
    Serial.println("Initializing BLE...");

    NimBLEDevice::init(deviceName);
    Serial.println("BLE device initialized");

    pServer = NimBLEDevice::createServer();
    Serial.println("BLE server created");

    customService = pServer->createService("0b60ab11-bc40-4d00-9ea4-5f2406872d9f");
    Serial.println("Service created");

    commandChar = customService->createCharacteristic(
        "cfa93afb-2c3d-4a76-a182-67e8b6d50b55",
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY
    );
    Serial.println("Characteristic created");

    commandCallbacks = new CommandCallbacks(this);
    commandChar->setCallbacks(commandCallbacks);
    Serial.println("Callbacks set");

    customService->start();
    Serial.println("Service started");

    serverCallbacks = new ServerCallbacks(this);
    pServer->setCallbacks(serverCallbacks);
    Serial.println("Server callbacks set");

    pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName(deviceName);
    pAdvertising->setAppearance(0x00C0);
    pAdvertising->addServiceUUID(customService->getUUID());
    pAdvertising->start();
    Serial.println("Advertising started");
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

    WriteBuffer buffer;
    packet.encode(buffer);
    std::vector<uint8_t> encodedData = buffer.getData();

    const int maxChunkSize = 18; // 20 bytes MTU - 2 bytes for header
    int totalLength = encodedData.size();
    int numFragments = (totalLength + maxChunkSize - 1) / maxChunkSize;

    if (numFragments > 15) {
        Serial.println("Packet too large for fragmentation");
        return;
    }

    uint8_t header[2];
    header[0] = static_cast<uint8_t>(packet.getType());

    for (int i = 0; i < numFragments; i++) {
        header[1] = (i << 4) | numFragments;

        std::vector<uint8_t> fragment;
        fragment.push_back(header[0]);
        fragment.push_back(header[1]);

        int start = i * maxChunkSize;
        int end = std::min(start + maxChunkSize, totalLength);
        fragment.insert(fragment.end(), encodedData.begin() + start, encodedData.begin() + end);

        commandChar->setValue(fragment.data(), fragment.size());
        commandChar->notify();
        delay(10);
    }

    Serial.println("Sent packet.");
}

void BluetoothManager::listen(PacketType type, std::function<void(const Packet &)> callback) {
    listeners[type] = callback;
}

void BluetoothManager::ServerCallbacks::onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) {
    Serial.println("Client connected");
    if (manager->connectCallback) {
        manager->connectCallback(pServer);
    }
}

void BluetoothManager::ServerCallbacks::onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) {
    Serial.print("Client disconnected, reason: ");
    Serial.println(reason);
    if (manager->disconnectCallback) {
        manager->disconnectCallback(pServer);
    }
    manager->startAdvertising();
    Serial.println("Restarted advertising");
}

void BluetoothManager::CommandCallbacks::onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) {
    std::string value = pCharacteristic->getValue();
    if (value.length() < 2) return; // Need at least 2 bytes for header

    uint8_t packetTypeByte = value[0];
    uint8_t fragHeader = value[1];
    uint8_t fragmentIndex = (fragHeader >> 4) & 0x0F;
    uint8_t totalFragments = fragHeader & 0x0F;

    const uint8_t* payload = reinterpret_cast<const uint8_t*>(value.data()) + 2;
    size_t payloadLength = value.length() - 2;

    if (fragmentIndex == 0) {
        manager->currentPacketType = static_cast<PacketType>(packetTypeByte);
        manager->fragmentBuffer.clear();
        manager->fragmentBuffer.insert(manager->fragmentBuffer.end(), payload, payload + payloadLength);
        manager->expectedFragments = totalFragments;
        manager->receivedFragments = 1;
    } else {
        // subsequent fragments
        if (fragmentIndex == manager->receivedFragments) {
            manager->fragmentBuffer.insert(manager->fragmentBuffer.end(), payload, payload + payloadLength);
            manager->receivedFragments++;
        } else {
            // out of order fragment; reset
            manager->fragmentBuffer.clear();
            manager->expectedFragments = 0;
            manager->receivedFragments = 0;
            manager->currentPacketType = static_cast<PacketType>(0);
            return;
        }
    }

    // all fragments present?
    if (manager->receivedFragments == manager->expectedFragments) {
        std::unique_ptr<Packet> packet(createPacketFromType(manager->currentPacketType));
        if (!packet) {
            Serial.print("Unknown packet type: ");
            Serial.println(static_cast<uint8_t>(manager->currentPacketType));
            return;
        }

        ReadBuffer buffer(manager->fragmentBuffer.data(), manager->fragmentBuffer.size());
        packet->decode(buffer);

        Serial.print("Received packet with type: ");
        Serial.println(static_cast<uint8_t>(packet->getType()));

        auto it = manager->listeners.find(packet->getType());
        if (it != manager->listeners.end()) {
            it->second(*packet);
        } else {
            Serial.println("No listener registered for this packet type");
        }

        // reset fragmentation state
        manager->fragmentBuffer.clear();
        manager->expectedFragments = 0;
        manager->receivedFragments = 0;
        manager->currentPacketType = static_cast<PacketType>(0);
    }
}