#pragma once
#include "protocol/Packet.h"

class TimeSyncPacket : public Packet {
public:
    TimeSyncPacket();
    TimeSyncPacket(
        int64_t timestamp
    );

    PacketType getType() const override;
    void encode(WriteBuffer& buffer) const override;
    void decode(ReadBuffer& buffer) override;

    int64_t timestamp;
};
