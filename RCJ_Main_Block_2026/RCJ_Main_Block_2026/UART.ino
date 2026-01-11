#include "UART.h"
#include "IMU.h"

// ===== グローバル変数 =====
ReadPacketData0 rp0;
ReadPacketData1 rp1;
ReadPacketData2 rp2;

SendPacketData0 sp0;
SendPacketData1 sp1;
SendPacketData2 sp2;

// ===== utility =====
uint8_t calcChecksum(uint8_t* data, uint8_t len) {
  uint8_t sum = 0;
  for (int i = 0; i < len; i++) sum ^= data[i];
  return sum;
}

void flushUntilHeader(HardwareSerial *s) {
  while (s->available()) {
    if (s->read() == 0xAA) break;
  }
}

void initUART() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 26, 27);  //TX, RX // to IR_ESP32
  Serial2.begin(115200, SERIAL_8N1, 16, 17);  // to LINE_ArduinoNano
}

int ball_angle;
int ball_distance;
int16_t line_angle;
int16_t line_dis;

void UART() {
  ReadResult r0= readPacket(0, rp0);
  ReadResult r1= readPacket(1, rp1);
  ReadResult r2= readPacket(2, rp2);


  if (r0 == READ_OK) { // 正常処理
    
  }
  else {
    
  }

  if (r1 == READ_OK) { // 正常処理
    ball_angle = rp1.ball_angle;
    ball_distance = rp1.ball_distance;
    line_angle = rp1.line_angle;
    line_dis = rp1.line_dis;
    Serial.println(line_angle);
  }
  else {
    
  }

  if (r2 == READ_OK) { // 正常処理
  
  }
  else {
    
  }

  sp1.vx = main_v.x;
  sp1.vy = main_v.y;
  sp1.omega = rotatePID(0,0);
  sendPacket(1, sp1);
  // else if (r == READ_CHECKSUM_ERROR) {
  //   Serial.println("READ_CHECKSUM_ERROR");
  //   delay(500);
  // }
}