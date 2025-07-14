#ifndef PACKET_BASE_H
#define PACKET_BASE_H

#include <stdint.h>

struct __attribute__((packed)) DataPacket {
    uint8_t header[4];  // Magic header: 0x55, 0xAA, 0x55, 0xAA
    int mot;
    bool link;
    bool failsafe;
    bool lost_frame;
    uint16_t channels[16];
    int azimuth;
    int pitch;
};

#endif
