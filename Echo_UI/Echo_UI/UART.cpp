#include <Arduino.h>
#include "UART.h"
#include "echo.h"
#include <SerialPIO.h>


// ToHub to_hub;

// FromHub from_hub;
// FromHub data_hub;

// FromSub from_sub;
// FromSub data_sub;

void UART::init() {
  Serial.begin(115200);
  PIOSerial1.begin(115200);
}

void UART::update() {
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

// void UART::flushUntilHeader(HardwareSerial *s) {  // ヘッダーがくるまでデータを破棄し続ける
//   while (s->available()) {
//     if (s->read() == 0xAA) break;
//   }
// }

// void Data::updateData() {
// //  dp.robot_angle = gyro.getAngle();
// }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



