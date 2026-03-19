#include "Instance.h"
#include "UART.h"

SerialPIO PIOSerial1(1, 0); // Main

void UART::initUART() {
  Serial.begin(115200); // シリアルモニター用
  PIOSerial1.begin(115200);
}

void UART::updateUART() {
  if (readPacket(PIOSerial1, from_main) == UART::READ_OK) {
    data.dp.ball_angle = from_main.ball_angle;
    data.dp.line_angle = from_main.line_angle;
    data.dp.robot_angle = from_main.robot_angle;
    data.dp.goal_angle = from_main.goal_angle;
  }

  to_main.echo_0 = data.dp.echoValues[0];
  to_main.echo_1 = data.dp.echoValues[1];
  to_main.echo_2 = data.dp.echoValues[2];
  to_main.echo_3 = data.dp.echoValues[3];
  to_main.echo_4 = data.dp.echoValues[4];
  to_main.echo_5 = data.dp.echoValues[5];
  to_main.echo_6 = data.dp.echoValues[6];
  to_main.echo_7 = data.dp.echoValues[7];

  sendPacket(PIOSerial1, to_main);
}

void Data::updateData() {
  echo.updateEcho();
  for (int i = 0; i < 8; i++) {
    dp.echoValues[i] = echo.echoValues[i];
  }
}