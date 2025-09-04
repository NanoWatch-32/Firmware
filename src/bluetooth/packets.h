#pragma once

#include <ArduinoJson.h>

enum class PacketType : uint8_t {
    MEDIA_COMMAND = 0x01,
};

struct Packet {
    virtual ~Packet() = default;

    virtual PacketType getType() const = 0;
    virtual size_t getSize() const = 0;

    virtual void serialize(JsonDocument& doc) const = 0;
    virtual void deserialize(const JsonDocument& doc) = 0;
};

struct MediaCommandPacket : Packet {
    uint8_t action;

    MediaCommandPacket() : action(0) {}
    MediaCommandPacket(uint8_t action) : action(action) {}

    PacketType getType() const override { return PacketType::MEDIA_COMMAND; }
    size_t getSize() const override { return sizeof(action); }

    void serialize(JsonDocument& doc) const override {
        doc["action"] = action;
    }

    void deserialize(const JsonDocument& doc) override {
        action = doc["action"];
    }
};

inline Packet* createPacketFromType(PacketType type) {
    switch (type) {
        case PacketType::MEDIA_COMMAND:
            return new MediaCommandPacket();
        default:
            return nullptr;
    }
}