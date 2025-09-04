#include "ReadBuffer.h"

ReadBuffer::ReadBuffer(const uint8_t* data, size_t length) 
    : data(data), length(length), position(0) {}

uint8_t ReadBuffer::readByte() {
    checkBounds(1);
    return data[position++];
}

int32_t ReadBuffer::readInt() {
    checkBounds(4);
    int32_t value = data[position] | 
                   (data[position + 1] << 8) | 
                   (data[position + 2] << 16) | 
                   (data[position + 3] << 24);
    position += 4;
    return value;
}

float ReadBuffer::readFloat() {
    int32_t intValue = readInt();
    float floatValue;
    memcpy(&floatValue, &intValue, sizeof(float));
    return floatValue;
}

std::string ReadBuffer::readString() {
    int32_t len = readInt();
    checkBounds(len);
    std::string str(reinterpret_cast<const char*>(data + position), len);
    position += len;
    return str;
}

std::vector<uint8_t> ReadBuffer::readImage() {
    int32_t len = readInt();
    checkBounds(len);
    std::vector<uint8_t> image(data + position, data + position + len);
    position += len;
    return image;
}

bool ReadBuffer::hasMore() const {
    return position < length;
}

void ReadBuffer::checkBounds(size_t required) {
    if (position + required > length) {
        throw std::out_of_range("Read beyond buffer bounds");
    }
}