#include "MediaInfoPacket.h"
#include "protocol/WriteBuffer.h"
#include "protocol/ReadBuffer.h"

MediaInfoPacket::MediaInfoPacket()
    : position(0), isPlaying(false) {}

MediaInfoPacket::MediaInfoPacket(
    std::string title,
    std::string artist,
    std::string album,
    int64_t duration,
    int64_t position,
    bool isPlaying
) : title(title), artist(artist), album(album),
    duration(duration), position(position), isPlaying(isPlaying) {}

PacketType MediaInfoPacket::getType() const {
    return PacketType::MEDIA_INFO;
}

void MediaInfoPacket::encode(WriteBuffer& buffer) const {
    buffer.writeString(title);
    buffer.writeString(artist);
    buffer.writeString(album);
    buffer.writeLong(duration);
    buffer.writeLong(position);
    buffer.writeBoolean(isPlaying);
}

void MediaInfoPacket::decode(ReadBuffer& buffer) {
    title = buffer.readString();
    artist = buffer.readString();
    album = buffer.readString();
    duration = buffer.readLong();
    position = buffer.readLong();
    isPlaying = buffer.readBoolean();
}