#pragma once

#include "protocol/Packet.h"
#include "protocol/PacketType.h"

class MediaCommandPacket : public Packet {
public:
    MediaCommandPacket() = default;
    explicit MediaCommandPacket(int command);

    PacketType getType() const override;
    void encode(WriteBuffer &buffer) const override;
    void decode(ReadBuffer &buffer) override;

    int command;
};
