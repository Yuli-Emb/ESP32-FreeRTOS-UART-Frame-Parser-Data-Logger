#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include <stddef.h>

uint8_t calculateCRC8(const uint8_t* data, size_t length);
uint8_t calculateFrameCRC(uint16_t id, uint8_t dlc, const uint8_t* data);

#endif