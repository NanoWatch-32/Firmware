#include "Packet.h"
#include "PacketType.h"
#include "packet/MediaCommandPacket.h"
#include "packet/MediaInfoPacket.h"

Packet *createPacketFromType(PacketType type) {
    switch (type) {
        case PacketType::MEDIA_COMMAND:
            return new MediaCommandPacket();
        case PacketType::MEDIA_INFO:
            return new MediaInfoPacket();
        default:
            return nullptr;
    }
}
