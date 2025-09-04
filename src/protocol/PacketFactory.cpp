#include "Packet.h"
#include "PacketType.h"
#include "packet/MediaCommandPacket.h"

Packet *createPacketFromType(PacketType type) {
    switch (type) {
        case PacketType::MEDIA_COMMAND:
            return new MediaCommandPacket();
        default:
            return nullptr;
    }
}
