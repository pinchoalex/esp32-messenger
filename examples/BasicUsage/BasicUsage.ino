#include "Messanger.h"
#include "CustomPacket.h"

DataPacket packetSend;
DataPacket packetRead;

Messanger msg(UART_NUM_0);

void setup() {
    Serial.begin(115200);

    // Initialize Messenger with UART0 and assign packet pointers
    msg.begin(115200, 1, 3, &packetSend, &packetRead);
}

void loop() {
    // Fill data for transmission
    if (xSemaphoreTake(msg.sendMutex_, pdMS_TO_TICKS(5)) == pdTRUE) {
        memcpy(packetSend.header, msg.HEADER_MAGIC, 4);
        packetSend.rpm = random(1000, 5000);
        packetSend.temperature = random(200, 300) / 10.0;
        packetSend.active = true;
        xSemaphoreGive(msg.sendMutex_);
    }

    // Read received data
    if (xSemaphoreTake(msg.readMutex_, pdMS_TO_TICKS(5)) == pdTRUE) {
        Serial.print("Received RPM: ");
        Serial.println(packetRead.rpm);
        Serial.print("Temperature: ");
        Serial.println(packetRead.temperature);
        Serial.print("Active: ");
        Serial.println(packetRead.active);
        xSemaphoreGive(msg.readMutex_);
    }

    delay(500);
}