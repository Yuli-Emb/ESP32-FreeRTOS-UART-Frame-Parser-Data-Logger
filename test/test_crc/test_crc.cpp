#include <Arduino.h>
#include <unity.h>
#include "crc.h"

void crc_test_data () { // Test if predeterminate vector of data is calculated correctly
    const uint8_t data [] = "123456789";

    TEST_ASSERT_EQUAL_HEX8(0xF4, calculateCRC8(data, 9));
}

void crc_test_same () { // Test if same data array calculated twice gives equal results
    const uint8_t data [] = {0x12, 0x34, 0x3, 0xDE, 0xAD, 0xBE};

    uint8_t first = calculateCRC8(data, sizeof(data));
    uint8_t second = calculateCRC8(data, sizeof(data));

    TEST_ASSERT_EQUAL_HEX8(first, second);
}

void crc_test_diff () { // Test if different data arrays give different results
    const uint8_t data [] = {0x12, 0x34, 0xDE, 0xAD};
    const uint8_t data_diff [] = {0x12, 0x34, 0xBE, 0xEF};

    TEST_ASSERT_NOT_EQUAL(calculateCRC8(data, sizeof(data)), calculateCRC8(data_diff, sizeof(data)));
}

void setup() {
    delay(2000);

    UNITY_BEGIN();

    RUN_TEST(crc_test_data);
    RUN_TEST(crc_test_same);
    RUN_TEST(crc_test_diff);

    UNITY_END();
}

void loop() {
}