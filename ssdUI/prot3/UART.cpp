#include "UART.h"

ReadPacketData0 rp0;
SendPacketData0 sp0;

uint8_t calcChecksum(uint8_t* data, uint8_t len) {
    uint8_t sum = 0;
    for (int i = 0; i < len; i++) sum ^= data[i];
    return sum;
}

void initUART() {
    Serial.begin(115200);
}

void UART() {
    readPacket(0, rp0);
}
