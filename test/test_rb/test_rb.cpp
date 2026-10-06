#include <Arduino.h>
#include <unity.h>
#include "ring_buffer.h"

static Ring_Buffer rb;

void rb_test_empty() { // Read from empty buffer
    uint8_t byte = 0xFF;
    
    TEST_ASSERT_EQUAL_UINT8(1, rb_read(&rb, &byte));
}

void rb_test_one_wr () { // Write 1 byte and read it
    uint8_t byte = 0;

    TEST_ASSERT_EQUAL_UINT8(0, rb_write(&rb, 0xBE));
    TEST_ASSERT_EQUAL_UINT32(1, rb_available(&rb));

    TEST_ASSERT_EQUAL_UINT8(0, rb_read(&rb, &byte));
    TEST_ASSERT_EQUAL_UINT8(0xBE, byte);
    TEST_ASSERT_EQUAL_UINT32(0, rb_available(&rb));

}

void rb_test_multiple_wr () { // Write multiple bytes and read them (FIFO)
    uint8_t bytes[] = {0xDE, 0xAD, 0xBE, 0xEF};
    size_t len = sizeof(bytes);
    uint8_t byte = 0;

    for (int i = 0; i < len; i++){
        TEST_ASSERT_EQUAL_UINT8(0, rb_write(&rb, bytes[i]));
    }

    TEST_ASSERT_EQUAL_UINT32(len, rb_available(&rb));

    for (int i = 0; i < len; i++) {
        TEST_ASSERT_EQUAL_UINT8(0, rb_read(&rb, &byte));
        TEST_ASSERT_EQUAL_UINT8(bytes[i], byte);
    }

    TEST_ASSERT_EQUAL_UINT32(0, rb_available(&rb));
}   

void rb_test_full () { // Full ring buffer
    uint8_t max = RING_BUFFER_SIZE;
    for (int i = 0; i < max - 1; i++) {
        TEST_ASSERT_EQUAL_UINT8(0, rb_write(&rb, (uint8_t) i));
    }

    TEST_ASSERT_EQUAL_UINT32(max - 1, rb_available(&rb));
    TEST_ASSERT_EQUAL_UINT8(0, rb.overrun);
}

void rb_test_wrap () {
    uint8_t read_byte = 0;

    rb.head = RING_BUFFER_SIZE - 2;
    rb.tail = RING_BUFFER_SIZE - 2;

    TEST_ASSERT_EQUAL_UINT8(0, rb_write(&rb, 0xA1)); // 254
    TEST_ASSERT_EQUAL_UINT8(0, rb_write(&rb, 0xA2)); // 255
    TEST_ASSERT_EQUAL_UINT8(0, rb_write(&rb, 0xA3)); // Wrapped to 0
    TEST_ASSERT_EQUAL_UINT8(0, rb_write(&rb, 0xA4)); // Wrapped to 1

    TEST_ASSERT_EQUAL_UINT32(4, rb_available(&rb));

    TEST_ASSERT_EQUAL_UINT8(0, rb_read(&rb, &read_byte));
    TEST_ASSERT_EQUAL_UINT8(0xA1, read_byte);

    TEST_ASSERT_EQUAL_UINT8(0, rb_read(&rb, &read_byte));
    TEST_ASSERT_EQUAL_UINT8(0xA2, read_byte);

    TEST_ASSERT_EQUAL_UINT8(0, rb_read(&rb, &read_byte));
    TEST_ASSERT_EQUAL_UINT8(0xA3, read_byte);

    TEST_ASSERT_EQUAL_UINT8(0, rb_read(&rb, &read_byte));
    TEST_ASSERT_EQUAL_UINT8(0xA4, read_byte);

    TEST_ASSERT_EQUAL_UINT32(0, rb_available(&rb));    
}

void rb_test_stress () {
    uint8_t read_byte = 0;
    for (uint32_t cycle = 0; cycle < 1000; cycle++) {
        uint8_t val1 = (uint8_t)(cycle & 0xFF);
        uint8_t val2 = (uint8_t)((cycle + 1) & 0xFF);

        TEST_ASSERT_EQUAL_UINT8(0, rb_write(&rb, val1));
        TEST_ASSERT_EQUAL_UINT8(0, rb_write(&rb, val2));

        TEST_ASSERT_EQUAL_UINT8(0, rb_read(&rb, &read_byte));
        TEST_ASSERT_EQUAL_UINT8(val1, read_byte);

        TEST_ASSERT_EQUAL_UINT8(0, rb_read(&rb, &read_byte));
        TEST_ASSERT_EQUAL_UINT8(val2, read_byte);
    }

    TEST_ASSERT_EQUAL_UINT32(0, rb_available(&rb));
    TEST_ASSERT_EQUAL_UINT8(0, rb.overrun);
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(rb_test_empty);
    RUN_TEST(rb_test_one_wr);
    RUN_TEST(rb_test_multiple_wr);
    RUN_TEST(rb_test_full);
    UNITY_END();
}

void loop() {
}