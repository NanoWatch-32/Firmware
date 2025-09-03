#pragma once

#include <pb_encode.h>
#include <Arduino.h>

#include "pb_decode.h"
#include "proto_gen/protocol.pb.h"

inline int encode_command_packet(const nano_CommandPacket *packet, uint8_t *buffer, size_t buffer_size) {
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);
    if (!pb_encode(&stream, nano_CommandPacket_fields, packet)) {
        Serial.print("Encoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return -1;
    }
    return static_cast<int>(stream.bytes_written);
}

inline bool decode_command_packet(const uint8_t* buffer, size_t length, nano_CommandPacket* packet) {
    pb_istream_t stream = pb_istream_from_buffer(buffer, length);
    if (!pb_decode(&stream, nano_CommandPacket_fields, packet)) {
        Serial.print("Decoding failed: ");
        Serial.println(PB_GET_ERROR(&stream));
        return false;
    }
    return true;
}