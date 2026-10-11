#ifndef CAN_FRAME_H
#define CAN_FRAME_H

#include <stdint.h>

#define MAX_FRAME_SIZE 8

typedef struct {
    uint16_t id;
    uint8_t dlc;
    uint8_t data[MAX_FRAME_SIZE];
    uint32_t timestamp;
} CAN_frame_t;

#endif