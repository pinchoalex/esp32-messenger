#include "Messanger.h"
#include <Arduino.h>
#include <string.h>

Messanger::Messanger(uart_port_t uart_num)
    : uart_num_(uart_num), packetSend_(nullptr), packetRead_(nullptr),
      readMutex_(nullptr), sendMutex_(nullptr) {}

void Messanger::begin(int baud_rate, int tx_pin, int rx_pin, DataPacket* send, DataPacket* read) {
    uart_config_t config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(uart_num_, &config);
    uart_set_pin(uart_num_, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_num_, 1024 * 2, 0, 0, NULL, 0);

    packetSend_ = send;
    packetRead_ = read;

    readMutex_ = xSemaphoreCreateMutex();
    sendMutex_ = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(receivePacketTask, "RecvTask", 4096, this, 1, NULL, 1);
    xTaskCreatePinnedToCore(sendPacketTask, "SendTask", 4096, this, 1, NULL, 1);
}

void Messanger::send(const DataPacket& packet) {
    uart_write_bytes(uart_num_, reinterpret_cast<const char*>(&packet), sizeof(DataPacket));
}

bool Messanger::receive(DataPacket& packet) {
    const size_t totalSize = sizeof(DataPacket);
    uint8_t* buffer = reinterpret_cast<uint8_t*>(&packet);
    uint8_t headerBuf[4] = {0};

    while (true) {
        headerBuf[0] = headerBuf[1];
        headerBuf[1] = headerBuf[2];
        headerBuf[2] = headerBuf[3];
        uart_read_bytes(uart_num_, &headerBuf[3], 1, pdMS_TO_TICKS(10));

        if (memcmp(headerBuf, HEADER_MAGIC, 4) == 0) {
            buffer[0] = headerBuf[0];
            buffer[1] = headerBuf[1];
            buffer[2] = headerBuf[2];
            buffer[3] = headerBuf[3];

            int readBytes = uart_read_bytes(uart_num_, buffer + 4, totalSize - 4, pdMS_TO_TICKS(50));
            return readBytes == (int)(totalSize - 4);
        }
    }
}

void Messanger::receivePacketTask(void* pvParameters) {
    Messanger* msg = reinterpret_cast<Messanger*>(pvParameters);
    while (true) {
        DataPacket temp;
        if (msg->receive(temp)) {
            if (memcmp(temp.header, msg->HEADER_MAGIC, 4) == 0) {
                if (xSemaphoreTake(msg->readMutex_, pdMS_TO_TICKS(10)) == pdTRUE) {
                    memcpy(msg->packetRead_, &temp, sizeof(DataPacket));
                    xSemaphoreGive(msg->readMutex_);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void Messanger::sendPacketTask(void* pvParameters) {
    Messanger* msg = reinterpret_cast<Messanger*>(pvParameters);
    while (true) {
        if (xSemaphoreTake(msg->sendMutex_, pdMS_TO_TICKS(10)) == pdTRUE) {
            msg->send(*(msg->packetSend_));
            xSemaphoreGive(msg->sendMutex_);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}