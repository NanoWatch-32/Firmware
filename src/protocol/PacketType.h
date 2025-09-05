#pragma once

#include <cstdint>

enum class PacketType : uint8_t {
    MEDIA_COMMAND = 0,
    MEDIA_INFO = 1,
};
