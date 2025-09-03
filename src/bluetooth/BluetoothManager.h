#pragma once
#include <NimBLEDevice.h>
#include <functional>
#include <string>
#include "proto_gen/protocol.pb.h"
#include <map>
#include "pb_encode.h"
#include "pb_decode.h"

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

    void send(const nano_CommandPacket &packet) const;

    void listen(int payloadTag, std::function<void(const nano_CommandPacket &)> callback);

private:
    NimBLEServer *pServer = nullptr;
    NimBLEAdvertising *pAdvertising = nullptr;
    NimBLEService *customService = nullptr;
    NimBLECharacteristic *commandChar = nullptr;

    std::function<void(NimBLEServer*)> connectCallback = nullptr;
    std::function<void(NimBLEServer*)> disconnectCallback = nullptr;

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

    std::map<int, std::function<void(const nano_CommandPacket &)>> listeners;
};

extern BluetoothManager bluetooth_manager;
