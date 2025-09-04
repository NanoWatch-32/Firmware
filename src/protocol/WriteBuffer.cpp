#include "WriteBuffer.h"

WriteBuffer::WriteBuffer() : position(0) {}

void WriteBuffer::writeByte(uint8_t value) {
    ensureCapacity(1);
    data[position++] = value;
}

void WriteBuffer::writeInt(int32_t value) {
    ensureCapacity(4);
    data[position++] = value & 0xFF;
    data[position++] = (value >> 8) & 0xFF;
    data[position++] = (value >> 16) & 0xFF;
    data[position++] = (value >> 24) & 0xFF;
}

void WriteBuffer::writeFloat(float value) {
    ensureCapacity(4);
    uint32_t intValue;
    memcpy(&intValue, &value, sizeof(float));
    writeInt(static_cast<int32_t>(intValue));
}

void WriteBuffer::writeString(const std::string& str) {
    writeInt(static_cast<int32_t>(str.length()));
    ensureCapacity(str.length());
    for (char c : str) {
        data[position++] = c;
    }
}

void WriteBuffer::writeImage(const std::vector<uint8_t>& image) {
    writeInt(static_cast<int32_t>(image.size()));
    ensureCapacity(image.size());
    for (uint8_t byte : image) {
        data[position++] = byte;
    }
}

const std::vector<uint8_t>& WriteBuffer::getData() const {
    return data;
}

void WriteBuffer::clear() {
    data.clear();
    position = 0;
}

void WriteBuffer::ensureCapacity(size_t additional) {
    if (position + additional > data.size()) {
        data.resize(position + additional);
    }
}