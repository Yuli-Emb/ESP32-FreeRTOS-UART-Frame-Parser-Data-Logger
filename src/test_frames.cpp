#include "test_frames.h"

void Valid_Frame () {
    uint8_t data[] = {
        0xDE, 0xAD, 0xBE
    };

    uint8_t frame[] = {0xAA, 0x07, 0xE8, 0x03, 0xDE, 0xAD, 0xBE, calculateFrameCRC(0x07E8, 3, data)};

    Serial2.write(frame, sizeof(frame));
}

void Corrupt_Frame () {
    uint8_t frame[] = {0xAA, 0x07, 0xE8, 0x03, 0xAA, 0xAF, 0xBB, 0xAA};

    Serial2.write(frame, sizeof(frame));
}

