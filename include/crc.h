#ifndef CAN_FRAME_H
#define CAN_FRAME_H

#include <stdint.h>
#include <stddef.h>

uint8_t calculateCRC8(const uint8_t* data, size_t length);

#endif