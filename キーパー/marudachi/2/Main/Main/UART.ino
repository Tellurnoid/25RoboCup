#include "UART.h"

ToHub to_hub;

FromHub from_hub;
FromHub data_hub;

FromSub from_sub;
FromSub data_sub;

void initUART() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 16, 17);
  Serial2.begin(115200, SERIAL_8N1, 18, 19);
}

void UART() {
  // if (readPacket(Serial2, from_sub) == READ_OK) {
  //   data_sub = from_sub;
  //   // Serial.println(data_sub.line_angle);
  // } 

  // if (readPacket(Serial1, from_hub) == READ_OK) {
  //   data_hub = from_hub;
  // }

  while (readPacket(Serial2, from_sub) == READ_OK) {
    data_sub = from_sub; // 最新値だけ残す
  }
  while (readPacket(Serial1, from_hub) == READ_OK) {
    data_hub = from_hub;
  }

  to_hub.vx = main_v.x;
  to_hub.vy = main_v.y;
  to_hub.omega = rotatePID(0, 0);
  to_hub.content = data_hub.move_x;
  sendPacket(Serial1, to_hub);
}