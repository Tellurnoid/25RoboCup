#include "UART.h"

// ===== グローバル変数 =====
ReadPacketData0 rp0;
ReadPacketData1 rp1;
ReadPacketData2 rp2;

SendPacketData0 sp0;
SendPacketData1 sp1;
SendPacketData2 sp2;

int dis_front;
int dis_back;
int dis_right;
int dis_left;
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
  Serial1.begin(115200, SERIAL_8N1, 18, 19);  //TX, RX // to IR_ESP32
  Serial2.begin(115200, SERIAL_8N1, 16, 17);  // to LINE_ArduinoNano
}

int ball_angle;
int ball_dis;
int cx;

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
    ball_dis = rp1.ball_distance;
    cx = rp1.cx;
  }
  else {
    
  }

  if (r2 == READ_OK) { // 正常処理
    line_angle = rp2.line_angle;
    line_dis = rp2.line_dis;
  }
  else {
    
  }
  // else if (r == READ_CHECKSUM_ERROR) {
  //   Serial.println("READ_CHECKSUM_ERROR");
  //   delay(500);
  // }

  // Serial.println(cx);
  //デバッグ用、削除可
    // Serial.print("A:");Serial.print(rp1.dis1);Serial.print("cm");
    // Serial.print("B:");Serial.print(rp1.dis2);Serial.print("cm");
    // Serial.print("C:");Serial.print(rp1.dis3);Serial.print("cm");
    // Serial.print("D:");Serial.print(rp1.dis4);Serial.println("cm");

////////////////////////////////////////////////////////////////////////////////
//割当て
  dis_front = rp1.dis2;
  dis_back = rp1.dis4;
  dis_right = rp1.dis3;
  dis_left = rp1.dis1;

///////////////////////////////////////////////////////////////////////////////////

    // Serial.print("A:");Serial.print(rp1.dis1);Serial.print("cm");
    // Serial.print("B:");Serial.print(rp1.dis2);Serial.print("cm");
    // Serial.print("C:");Serial.print(rp1.dis3);Serial.print("cm");
    // Serial.print("D:");Serial.print(rp1.dis4);Serial.println("cm");
}