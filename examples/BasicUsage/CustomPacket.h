#ifndef CUSTOM_PACKET_H
#define CUSTOM_PACKET_H

#include <stdint.h>

struct __attribute__((packed)) DataPacket {
    uint8_t header[4];  // Magic bytes
    int rpm;
    float temperature;
    bool active;
};

#endif