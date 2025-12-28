#ifndef UART_H
#define UART_H

#include <Arduino.h>

// ====== packet struct ======
struct ReadPacketData0 {
  int ball_angle;
  int goal_angle;
};

struct ReadPacketData1 {
  int ball_angle;
  int ball_distance;
  int ball_max;
  int16_t dis1;
  int16_t dis2;
  int16_t dis3;
  int16_t dis4;
  int16_t cx;
};

struct ReadPacketData2 {
  int16_t line_angle;
  int16_t line_dis;
};

struct SendPacketData0 {
  int ball_angle;
  int ball_distance;
};

struct SendPacketData1 {
  int angleZ;
};

struct SendPacketData2 {
  int x;
};

// ===== enum =====
enum ReadResult : uint8_t {
  READ_OK = 0,
  READ_NO_HEADER,
  READ_LEN_MISMATCH,
  READ_NOT_ENOUGH_DATA,
  READ_CHECKSUM_ERROR,
  READ_INVALID_SERIAL
};

// ===== function =====
uint8_t calcChecksum(uint8_t* data, uint8_t len);
void flushUntilHeader(HardwareSerial *s);
void initUART();
void UART();

// ===== template (必ずhに定義を書く) =====
template <typename readT>
ReadResult readPacket(int num_serial, readT &d) {

  HardwareSerial *seri;
  if (num_serial == 0)      seri = &Serial;
  else if (num_serial == 1) seri = &Serial1;
  else if (num_serial == 2) seri = &Serial2;
  else return READ_INVALID_SERIAL;

  int header = -1;
  while (seri->available()) {
    int c = seri->read();
    if (c == 0xAA) { header = c; break; }
  }
  if (header != 0xAA) return READ_NO_HEADER;

  if (seri->available() < 1) return READ_NOT_ENOUGH_DATA;
  uint8_t len = seri->read();

  if (len != sizeof(readT)) {
    flushUntilHeader(seri);
    return READ_LEN_MISMATCH;
  }

  if (seri->available() < len + 1) return READ_NOT_ENOUGH_DATA;

  uint8_t buf[sizeof(readT)];
  seri->readBytes(buf, len);

  uint8_t cs = seri->read();
  if (calcChecksum(buf, len) != cs) {
    flushUntilHeader(seri);
    return READ_CHECKSUM_ERROR;
  }

  memcpy(&d, buf, len);
  return READ_OK;
}

template <typename sendT>
void sendPacket(int num_serial, sendT &d) {

  HardwareSerial *seri;
  if (num_serial == 0) seri = &Serial;
  else if (num_serial == 1) seri = &Serial1;
  else seri = &Serial2;

  uint8_t* p = (uint8_t*)&d;
  uint8_t len = sizeof(sendT);
  uint8_t cs = calcChecksum(p, len);

  seri->write(0xAA);
  seri->write(len);
  seri->write(p, len);
  seri->write(cs);
}

extern int ball_angle;
extern int ball_dis;
extern int cx;

extern int16_t line_angle;
extern int16_t line_dis;

#endif
