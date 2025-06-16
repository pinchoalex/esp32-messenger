# esp32-messenger

**esp32-messenger** is a lightweight FreeRTOS-based UART communication library for ESP32.  
It allows sending and receiving structured binary data packets over UART between two devices using FreeRTOS tasks and semaphores.

---

## ✨ Features

- Non-blocking send/receive using FreeRTOS tasks
- Custom packet structure support
- Mutex-protected access to shared data
- Compatible with Arduino framework on ESP32

---

## 📦 Installation

Copy the `src/` folder into your project or install via PlatformIO as a local library:

```
lib_deps =
  https://github.com/pinchoalex/esp32-messenger
```

---

## 🧩 Define Your Own Packet Structure

To use your own structure, create a file named `PacketBase.h` and define a struct named `DataPacket`:

```cpp
// PacketBase.h
#ifndef PACKET_BASE_H
#define PACKET_BASE_H

#include <stdint.h>

struct __attribute__((packed)) DataPacket {
    uint8_t header[4];    // Magic bytes
    int rpm;
    float temperature;
    bool active;
};

#endif
```

> You can add any fields you want. Just make sure the struct is packed and called `DataPacket`.

---

## ✅ Basic Usage Example

```cpp
#include "Messanger.h"
#include "PacketBase.h"  // Your custom packet structure

DataPacket packetSend;
DataPacket packetRead;

Messanger msg(UART_NUM_0);

void setup() {
    Serial.begin(115200);
    
    // Initialize Messenger on UART0 with TX=1, RX=3 and your packets
    msg.begin(115200, 1, 3, &packetSend, &packetRead);
}

void loop() {
    // Fill data to send
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
```

---

## 🔧 Notes

- UART config is initialized via `begin(baud, txPin, rxPin, &sendPkt, &recvPkt)`
- You must fill the `header[4]` field with the correct magic bytes (`0x55, 0xAA, 0x55, 0xAA`)
- Use `xSemaphoreTake()`/`xSemaphoreGive()` to safely update shared packets

---

## 📜 License

MIT — use freely, modify, and distribute with credit.