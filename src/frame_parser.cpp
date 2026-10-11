#include "frame_parser.h"

int crc_errors = 0;

void frame_parser_init(FrameParser *parser)
{
    parser->state = WAIT_SOF;
    parser->data_index = 0;
    parser->frame.id = 0;
    parser->frame.dlc = 0;
    parser->frame.timestamp = 0;
}

bool frame_parser_feed(
    FrameParser *parser,
    uint8_t byte,
    CAN_frame_t *out_frame)
{
    switch (parser->state)
    {
        case WAIT_SOF:
            if (byte == 0xAA)
            {
                parser->state = READ_ID_H;
            }
            break;

        case READ_ID_H:
            parser->frame.id = (uint16_t)byte << 8;
            parser->state = READ_ID_L;
            break;

        case READ_ID_L:
            parser->frame.id |= byte;
            parser->state = READ_DLC;
            break;

        case READ_DLC:
            if (byte > MAX_FRAME_SIZE)
            {
                parser->state = WAIT_SOF;
                parser->data_index = 0;
                break;
            }

            parser->frame.dlc = byte;
            parser->data_index = 0;

            parser->state = (byte == 0) ? READ_CRC : READ_DATA;
            break;

        case READ_DATA:
            parser->frame.data[parser->data_index++] = byte;

            if (parser->data_index == parser->frame.dlc)
            {
                parser->state = READ_CRC;
            }
            break;

        case READ_CRC:
        {
            uint8_t expected_crc = calculateFrameCRC(
                parser->frame.id,
                parser->frame.dlc,
                parser->frame.data
            );

            bool valid = (byte == expected_crc);

            parser->state = WAIT_SOF;
            parser->data_index = 0;

            if (valid)
            {
                *out_frame = parser->frame;
                return true;
            }
            else {
                crc_errors++;
            }

            break;
        }
    }

    return false;
}