#ifndef FRAME_PARSER_H
#define FRAME_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include "can_frame.h"
#include "ring_buffer.h"
#include "crc.h"

extern int crc_errors;

typedef enum {
    WAIT_SOF,
    READ_ID_H,
    READ_ID_L,
    READ_DLC,
    READ_DATA,
    READ_CRC
} FrameState;

typedef struct {
    FrameState state;
    CAN_frame_t frame;
    uint8_t data_index;
} FrameParser;

void frame_parser_init(FrameParser *parser);

bool frame_parser_feed(
    FrameParser *parser,
    uint8_t byte,
    CAN_frame_t *out_frame
);

#endif