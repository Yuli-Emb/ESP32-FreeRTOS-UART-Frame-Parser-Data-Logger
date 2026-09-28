#include "crc.h"

uint8_t calculateCRC8(const uint8_t* data, size_t length)
{
    uint8_t crc = 0x00;

    for (size_t i = 0; i < length; i++)
    {
        crc ^= data[i];

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }

    return crc;
}

uint8_t calculateFrameCRC(uint16_t id, uint8_t dlc, const uint8_t* data)
{
    uint8_t bytes[3 + 8];

    bytes[0] = (id >> 8) & 0xFF;
    bytes[1] = id & 0xFF;
    bytes[2] = dlc;

    for (uint8_t i = 0; i < dlc; i++)
        bytes[3 + i] = data[i];

    return calculateCRC8(bytes, 3 + dlc);
}