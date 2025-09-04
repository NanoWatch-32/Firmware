#pragma once
#include <NimBLEDevice.h>
#include <functional>
#include <string>
#include <map>
#include <memory>
#include "packets.h"

class BluetoothManager {
public:
    BluetoothManager();

    void begin(const std::string &deviceName);
    void setPin(const std::string &pin);

    void startAdvertising();
    void stopAdvertising();

    void onConnect(std::function<void(NimBLEServer*)> callback);
    void onDisconnect(std::function<void(NimBLEServer*)> callback);

    bool isConnected() const;
    bool isAdvertising() const;

    void send(const Packet& packet) const;
    void listen(PacketType type, std::function<void(const Packet&)> callback);

private:
    NimBLEServer *pServer = nullptr;
    NimBLEAdvertising *pAdvertising = nullptr;
    NimBLEService *customService = nullptr;
    NimBLECharacteristic *commandChar = nullptr;

    std::function<void(NimBLEServer*)> connectCallback = nullptr;
    std::function<void(NimBLEServer*)> disconnectCallback = nullptr;

    // Fragmentation state
    mutable String fragmentBuffer;
    mutable uint8_t expectedFragments = 0;
    mutable uint8_t receivedFragments = 0;
    mutable PacketType currentPacketType = static_cast<PacketType>(0);

    class ServerCallbacks : public NimBLEServerCallbacks {
    public:
        ServerCallbacks(BluetoothManager *manager) : manager(manager) {}
        void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override;
        void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override;
    private:
        BluetoothManager *manager;
    };

    class CommandCallbacks : public NimBLECharacteristicCallbacks {
    public:
        CommandCallbacks(BluetoothManager *manager) : manager(manager) {}
        void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override;
    private:
        BluetoothManager *manager;
    };

    ServerCallbacks *serverCallbacks = nullptr;
    CommandCallbacks *commandCallbacks = nullptr;

    std::map<PacketType, std::function<void(const Packet&)>> listeners;
};

extern BluetoothManager bluetooth_manager;