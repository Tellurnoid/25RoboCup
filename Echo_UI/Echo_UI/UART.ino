#include "UART.h"
#include <SerialPIO.h>

SerialPIO PIOSerial1(1, 0); // TX, RX

RxMainPacket rx_main;
TxMainPacket tx_main;

RxMainPacket date_main;

void initUART() {
  Serial.begin(115200);
  PIOSerial1.begin(115200);
}

void UART() {
  if (readPacket(ID_UI, PIOSerial1, rx_main) == READ_OK) {
    date_main = rx_main;
    Serial.println(date_main.test);
  }
  else {
  }
}