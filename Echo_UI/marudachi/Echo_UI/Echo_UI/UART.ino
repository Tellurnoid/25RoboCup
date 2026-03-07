#include "UART.h"
#include <SerialPIO.h>

SerialPIO PIOSerial1(1, 0); // TX, RX


FromMain from_main;
FromMain data_main;

ToMain to_main;

void initUART() {
  Serial.begin(115200);
  PIOSerial1.begin(115200);
}

void UART() {
  if (readPacket(PIOSerial1, from_main) == READ_OK) {
    data_main = from_main;
    Serial.println(from_main.content);
  }
  else {
    // Serial.print(readPacket(ID_HUB_to_UI, PIOSerial1, rx_ui));
  }
  to_main.echo_0 = echo.echoValues[0];
  to_main.echo_1 = echo.echoValues[1];
  to_main.echo_2 = echo.echoValues[2];
  to_main.echo_3 = echo.echoValues[3];
  to_main.echo_4 = echo.echoValues[4];
  to_main.echo_5 = echo.echoValues[5];
  to_main.echo_6 = echo.echoValues[6];
  to_main.echo_7 = echo.echoValues[7];

  to_main.echo_7 = echo.echoValues[7];

  sendPacket(PIOSerial1, to_main);
}