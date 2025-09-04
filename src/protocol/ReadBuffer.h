#pragma once

#include <cstring>
#include <vector>
#include <string>
#include <stdexcept>

class ReadBuffer {
public:
    ReadBuffer(const uint8_t* data, size_t length);
    uint8_t readByte();
    int32_t readInt();
    float readFloat();
    int64_t readLong();
    bool readBoolean();
    std::string readString();
    std::vector<uint8_t> readImage();
    bool hasMore() const;

private:
    void checkBounds(size_t required);

    const uint8_t* data;
    size_t length;
    size_t position;
};