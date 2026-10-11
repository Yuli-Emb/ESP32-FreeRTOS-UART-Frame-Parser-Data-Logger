#include <Arduino.h>
#include <string.h>
#include "ring_buffer.h"
#include "can_frame.h"
#include "crc.h"
#include "test_frames.h"
#include "frame_parser.h"

#define UART_BAUD_RATE 115200
#define QUEUE_SIZE 5

SemaphoreHandle_t xSemaphore; // Declares semaphore
QueueHandle_t xQueue; // Holds reference to a queue. Declares a variable
Ring_Buffer rb;

volatile uint32_t frames_received = 0;
volatile uint32_t frames_dropped = 0;

void myISR() {
    while (Serial2.available()) {
        uint8_t byte = Serial2.read();
        rb_write(&rb, byte);
    }
    BaseType_t higherPriorityWoken = pdFALSE;
    xSemaphoreGiveFromISR(xSemaphore, &higherPriorityWoken);
    portYIELD_FROM_ISR(higherPriorityWoken);
}


void vReaderTask(void *pvParameters)
{
    FrameParser parser;
    CAN_frame_t frame;
    uint8_t byte;

    frame_parser_init(&parser);

    while (true)
    {
        xSemaphoreTake(xSemaphore, portMAX_DELAY);

        while (rb_available(&rb) != 0)
        {
            if (rb_read(&rb, &byte))
                continue;

            if (frame_parser_feed(&parser, byte, &frame))
            {
                frame.timestamp =
                    pdTICKS_TO_MS(xTaskGetTickCount());

                if (xQueueSend(xQueue, &frame, 0) == pdTRUE)
                {
                    frames_received++;
                }
                else
                {
                    frames_dropped++;
                }
            }
        }
    }
}

void vLoggerTask(void *pvParameters) { // Logs
    CAN_frame_t frame;
    while(1){
        xQueueReceive(xQueue, &frame, portMAX_DELAY);
        Serial.print("\n>> Frame\n"); // Output frame contents
        Serial.printf("[%lu ms] | ID:0x%03X | DLC:%d\n", frame.timestamp, frame.id, frame.dlc);
        Serial.printf("DATA: ");
        for (int i = 0; i < frame.dlc; i++) {
            Serial.printf("%02X ", frame.data[i]);
        }
        Serial.printf("\n");
        vTaskDelay(pdMS_TO_TICKS(100)); // deleay
    }
}

void vSimulatorTask(void *pvParameters) { // Frame Simulation
    vTaskDelay(pdMS_TO_TICKS(200));
    int flag = 0;
    while(1){
        //if (flag == 0) {
            Valid_Frame();
            //flag = 1;
        //}
        //else {
            //Corrupt_Frame();
            //flag = 0;
        //}
        vTaskDelay(pdMS_TO_TICKS(2500)); // Delay
    }
}

void vStatsTask(void *pvParameters) { // Statistics
    vTaskDelay(pdMS_TO_TICKS(200));
    while(1){
        Serial.printf("\n>> STATS\nReceived: %lu\nDropped: %lu\nQueue depth: %u\nCRC Errors: %u\n", frames_received, frames_dropped, uxQueueMessagesWaiting(xQueue), crc_errors); // Outputs statistics
        Serial.printf("Overrun: %d\n", rb.overrun);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void setup() {
    Serial.begin(UART_BAUD_RATE); // Baud - symbols per second
    

    Serial2.begin(UART_BAUD_RATE); // 
    Serial2.onReceive(myISR); // call this function when something happens

    xSemaphore = xSemaphoreCreateBinary();

    memset(&rb, 0, sizeof(Ring_Buffer));

    xQueue = xQueueCreate(QUEUE_SIZE, sizeof(CAN_frame_t));
    // 5 - how many iterms the queue can hold at once
    // sizeof - size of each item in bytes

    xTaskCreatePinnedToCore(vReaderTask, "ReaderTask", 5000, NULL, 3, NULL, 1);
    // Creates and starts task
    // Allocates the stack memory (2048)
    // Registers the function as task with the priority set (3)
    // Starts scheduling (will run as soon as scheduler gets control)
    // NULL - parameter to pass in and where to store the task handle
    // 1 - which core to run on

    xTaskCreatePinnedToCore(vLoggerTask, "LoggerTask", 5000, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(vSimulatorTask, "SimulatorTask", 5000, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(vStatsTask, "StatsTask", 5000, NULL, 1, NULL, 1);
}

void loop() {
    // empty
}