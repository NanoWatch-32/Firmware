#include "MediaCommandPacket.h"

#include "protocol/WriteBuffer.h"
#include "protocol/ReadBuffer.h"


MediaCommandPacket::MediaCommandPacket(int command) : command(command) {}

PacketType MediaCommandPacket::getType() const {
    return PacketType::MEDIA_COMMAND;
}

void MediaCommandPacket::encode(WriteBuffer &buffer) const {
    buffer.writeInt(command);
}

void MediaCommandPacket::decode(ReadBuffer &buffer) {
    command = buffer.readInt();
}

