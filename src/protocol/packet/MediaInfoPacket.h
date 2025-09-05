#pragma once
#include <string>

#include "protocol/Packet.h"
#include "protocol/PacketType.h"

class MediaInfoPacket : public Packet {
public:
    MediaInfoPacket();
    MediaInfoPacket(
        std::string title,
        std::string artist,
        std::string album,
        int64_t duration,
        int64_t position,
        bool isPlaying
    );

    PacketType getType() const override;
    void encode(WriteBuffer& buffer) const override;
    void decode(ReadBuffer& buffer) override;

    std::string title;
    std::string artist;
    std::string album;
    int64_t duration;
    int64_t position;
    bool isPlaying;
};

