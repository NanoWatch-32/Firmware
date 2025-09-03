#include "BluetoothManager.h"
#include <flatbuffers/flatbuffers.h>

#include "helper.h"

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
    if (pAdvertising) {
        pAdvertising->start();
    }
}

void BluetoothManager::stopAdvertising() {
    if (pAdvertising) {
        pAdvertising->stop();
    }
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

void BluetoothManager::listen(int payloadTag, std::function<void(const nano_CommandPacket &)> callback) {
    listeners[payloadTag] = callback;
}

void BluetoothManager::send(const nano_CommandPacket &packet) const {
    if (!isConnected()) {
        Serial.println("Cannot send: Not connected.");
        return;
    }

    uint8_t buffer[256];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    if (!pb_encode(&stream, nano_CommandPacket_fields, &packet)) {
        Serial.print("Encoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return;
    }

    Serial.println("Sent packet.");

    commandChar->setValue(buffer, stream.bytes_written);
    commandChar->notify();
}

void BluetoothManager::ServerCallbacks::onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) {
    if (manager->connectCallback) {
        manager->connectCallback(pServer);
    }
}

void BluetoothManager::ServerCallbacks::onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) {
    if (manager->disconnectCallback) {
        manager->disconnectCallback(pServer);
    }
    manager->startAdvertising();
}

void BluetoothManager::CommandCallbacks::onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) {
    std::string value = pCharacteristic->getValue();
    const uint8_t *data = reinterpret_cast<const uint8_t *>(value.data());
    size_t len = value.size();

    if (len == 0) return;

    nano_CommandPacket packet = nano_CommandPacket_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(data, len);

    bool status = pb_decode(&stream, nano_CommandPacket_fields, &packet);
    if (!status) {
        Serial.println("Failed to decode command packet");
        return;
    }

    Serial.print("Received BLE packet with payload tag: ");
    Serial.println(packet.which_payload);

    auto it = manager->listeners.find(packet.which_payload);
    if (it != manager->listeners.end()) {
        it->second(packet);
    } else {
        Serial.println("No listener registered for this payload tag");
    }
}
