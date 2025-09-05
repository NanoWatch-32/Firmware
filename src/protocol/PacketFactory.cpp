#include "Packet.h"
#include "PacketType.h"
#include "packet/MediaCommandPacket.h"
#include "packet/MediaInfoPacket.h"
#include "packet/TimeSyncPacket.h"

Packet *createPacketFromType(PacketType type) {
    switch (type) {
        case PacketType::MEDIA_COMMAND:
            return new MediaCommandPacket();
        case PacketType::MEDIA_INFO:
            return new MediaInfoPacket();
        case PacketType::TIME_SYNC:
            return new TimeSyncPacket();
        default:
            return nullptr;
    }
}
