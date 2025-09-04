#pragma once

#include "PacketType.h"

class WriteBuffer;
class ReadBuffer;

class Packet {
public:
    virtual ~Packet() = default;
    virtual PacketType getType() const = 0;
    virtual void encode(WriteBuffer& buffer) const = 0;
    virtual void decode(ReadBuffer& buffer) = 0;
};

Packet* createPacketFromType(PacketType type);