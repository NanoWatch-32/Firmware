#pragma once

#include <cstring>
#include <string>
#include <vector>

class WriteBuffer {
public:
    WriteBuffer();
    void writeByte(uint8_t value);
    void writeInt(int32_t value);
    void writeFloat(float value);
    void writeString(const std::string& str);
    void writeImage(const std::vector<uint8_t>& image);
    const std::vector<uint8_t>& getData() const;
    void clear();

private:
    void ensureCapacity(size_t additional);

    std::vector<uint8_t> data;
    size_t position;
};