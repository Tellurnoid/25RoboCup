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
  int16_t line_angle;
  int16_t line_dis;
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
  float vx;
  float vy;
  int omega;
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

  // UARTごとに状態を分離する
  static uint8_t buf[2][sizeof(readT)];
  static uint8_t idx[2] = {0, 0};
  static uint8_t len[2] = {0, 0};
  static uint8_t state[2] = {0, 0};

  uint8_t &st = state[num_serial];
  uint8_t &i  = idx[num_serial];
  uint8_t &l  = len[num_serial];
  uint8_t *b  = buf[num_serial];

  enum {
    WAIT_HEADER,
    WAIT_LEN,
    WAIT_DATA,
    WAIT_CHECKSUM
  };

  while (seri->available()) {
    uint8_t c = seri->read();

    switch (st) {
      case WAIT_HEADER:
        if (c == 0xAA) st = WAIT_LEN;
        break;

      case WAIT_LEN:
        if (c == sizeof(readT)) {
          l = c;
          i = 0;
          st = WAIT_DATA;
        } else {
          st = WAIT_HEADER;
          return READ_LEN_MISMATCH;
        }
        break;

      case WAIT_DATA:
        b[i++] = c;
        if (i >= l) st = WAIT_CHECKSUM;
        break;

      case WAIT_CHECKSUM:
        st = WAIT_HEADER;
        if (calcChecksum(b, l) == c) {
          memcpy(&d, b, l);
          return READ_OK;
        } else {
          return READ_CHECKSUM_ERROR;
        }
    }
  }
  return READ_NOT_ENOUGH_DATA;
}

// template <typename readT>
// ReadResult readPacket(int num_serial, readT &d) {

//   HardwareSerial *seri;
//   if (num_serial == 0)      seri = &Serial;
//   else if (num_serial == 1) seri = &Serial1;
//   else if (num_serial == 2) seri = &Serial2;
//   else return READ_INVALID_SERIAL;

//   int header = -1;
//   while (seri->available()) {
//     int c = seri->read();
//     if (c == 0xAA) { header = c; break; }
//   }
//   if (header != 0xAA) return READ_NO_HEADER;

//   if (seri->available() < 1) return READ_NOT_ENOUGH_DATA;
//   uint8_t len = seri->read();

//   if (len != sizeof(readT)) {
//     flushUntilHeader(seri);
//     return READ_LEN_MISMATCH;
//   }

//   if (seri->available() < len + 1) return READ_NOT_ENOUGH_DATA;

//   uint8_t buf[sizeof(readT)];
//   seri->readBytes(buf, len);

//   uint8_t cs = seri->read();
//   if (calcChecksum(buf, len) != cs) {
//     flushUntilHeader(seri);
//     return READ_CHECKSUM_ERROR;
//   }

//   memcpy(&d, buf, len);
//   return READ_OK;
// }

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
extern int ball_distance;
extern int16_t line_angle;
extern int16_t line_dis;

#endif
