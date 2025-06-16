#ifndef MESSANGER_H
#define MESSANGER_H

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "PacketBase.h"

class Messanger {
public:
    Messanger(uart_port_t uart_num = UART_NUM_1);

    void begin(int baud_rate, int tx_pin, int rx_pin, DataPacket* send, DataPacket* read);
    void send(const DataPacket& packet);
    bool receive(DataPacket& packet);

    const uint8_t HEADER_MAGIC[4] = {0x55, 0xAA, 0x55, 0xAA};

    SemaphoreHandle_t readMutex_;
    SemaphoreHandle_t sendMutex_;

private:
    static void receivePacketTask(void* pvParameters);
    static void sendPacketTask(void* pvParameters);

    uart_port_t uart_num_;
    DataPacket* packetSend_;
    DataPacket* packetRead_;
};

#endif