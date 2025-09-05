#include "TimeSyncPacket.h"

#include "protocol/ReadBuffer.h"
#include "protocol/WriteBuffer.h"

TimeSyncPacket::TimeSyncPacket()
    : timestamp(0) {}

TimeSyncPacket::TimeSyncPacket(
    int64_t timestamp
) : timestamp(timestamp) {}

PacketType TimeSyncPacket::getType() const {
    return PacketType::TIME_SYNC;
}

void TimeSyncPacket::encode(WriteBuffer &buffer) const {
    buffer.writeLong(timestamp);
}

void TimeSyncPacket::decode(ReadBuffer &buffer) {
    timestamp = buffer.readLong();
}

