#pragma once

//-----------------------------------------------------------------------------------------------
enum ID : uint8_t {
  NO_COMMAND = 0,
  MAIN,
  SUB,
  MOTOR,
  CAMERA
};

enum Main_UI : uint8_t {
  START = 0
};

struct __attribute__((packed)) FromSub {  // attributeでパディング(データの途中になんか入る)のを禁止、sizeOf Tが壊れないようにするため
  int16_t ball_angle;
  int16_t ball_distance;
  int16_t line_angle;
  int16_t line_distance;
};
extern FromSub from_sub;
extern FromSub data_sub;

struct __attribute__((packed)) ToSub {
  int16_t ball_angle;
  int16_t ball_distance;
  int16_t line_angle;
};

struct __attribute__((packed)) FromHub {  // attributeでパディング(データの途中になんか入る)のを禁止、sizeOf Tが壊れないようにするため
  int16_t vx;
  int16_t vy;
  int16_t move_x;
  int16_t move_y;
  
  int16_t echo_0;
  int16_t echo_1;
  int16_t echo_2;
  int16_t echo_3;
  int16_t echo_4;
  int16_t echo_5;
  int16_t echo_6;
  int16_t echo_7;

  uint8_t id;
  uint8_t ack;
  uint8_t content_id;
  int16_t content;
};
extern FromHub from_hub;
extern FromHub data_hub;

struct __attribute__((packed)) ToHub {
  float vx;
  float vy;
  int16_t omega;
  int16_t content;
};
extern ToHub to_hub;
//-----------------------------------------------------------------------------------------------
// ==== utility ====

uint8_t calcChecksum(uint8_t* data, uint8_t len) {
  uint8_t sum = 0;
  for (int i = 0; i < len; i++) sum ^= data[i];
  return sum;
}

void flushUntilHeader(HardwareSerial *s) {  // ヘッダーがくるまでデータを破棄し続ける
  while (s->available()) {
    if (s->read() == 0xAA) break;
  }
}

// void flushSerialIfOverflow(HardwareSerial &seri, size_t maxAllowed = 64) {
//     if (seri.available() > maxAllowed) {
//         while (seri.available()) seri.read();  // 全部読み捨て
//         Serial.println("Serial buffer overflow, flushed!");
//     }
// }

// ==== sendUART ====
template <typename sendT>
void sendPacket(HardwareSerial &seri, const sendT &d) { // *ポインタ、ではなく&参照で安全に
  // static_assert(condition, "message"); 通信バイトが255バイトを超えるとコンパイルエラーでmessageを表示する
  static_assert(sizeof(sendT) <= 255, "Packet too large");

  const uint8_t* p = reinterpret_cast<const uint8_t*>(&d);  // Serial.writeに必要なunit8_t*の型に、reinterpret_castで変換する
  const uint8_t len = sizeof(sendT);

  uint8_t cs = len;  // checkSumにid、len、中身のデータをかける
  for (uint8_t i = 0; i < len; i++) {
    cs ^= p[i];
  }

  seri.write(0xAA); // 参照 、ポインタの場合はseri->write();
  seri.write(0x55);
  seri.write(len);
  seri.write(p, len);
  seri.write(cs);
}

// ==== readUART ====

enum ReadResult : uint8_t {
  READ_OK = 0,
  READ_NO_HEADER,
  READ_ID_MISMATCH,
  READ_LEN_MISMATCH,
  READ_NOT_ENOUGH_DATA,
  READ_CHECKSUM_ERROR,
  READ_INVALID_SERIAL
};

// template <typename readT>
// ReadResult readPacket(HardwareSerial &seri, readT &d) {

//   enum State {
//     WAIT_HEADER,
//     WAIT_HEADER2,
//     WAIT_LEN,
//     WAIT_DATA,
//     WAIT_CHECKSUM
//   };

//   static uint8_t buf[sizeof(readT)];
//   static uint8_t idx = 0;
//   static uint8_t len = 0;
//   static State state = WAIT_HEADER;

//   while(seri.available()) {
//     // Serial.println(state);

//     uint8_t c = seri.read();

//     switch (state) {

//       case WAIT_HEADER:
//         if (c == 0xAA) state = WAIT_HEADER2;
//         break;

//       case WAIT_HEADER2:
//         if (c == 0x55) {
//           state = WAIT_LEN;
//         } else if (c == 0xAA){  // 連続AAの場合はヘッダーの可能性があるのでHEADER2に移行
//           state = WAIT_HEADER2;
//         } else {
//           state = WAIT_HEADER;
//         }
//         break;
//       case WAIT_LEN:
//         if (c == sizeof(readT)) {
//           len = c;
//           idx = 0;
//           state = WAIT_DATA;
//         } else {
//           state = WAIT_HEADER;
//           return READ_LEN_MISMATCH;
//         }
//         break;

//       case WAIT_DATA:
//         buf[idx++] = c;
//         if (idx >= len) state = WAIT_CHECKSUM;
//         break;

//       case WAIT_CHECKSUM: {
//         uint8_t cs = len;
//         for (uint8_t i = 0; i < len; i++) {
//           cs ^= buf[i];
//         }

//         state = WAIT_HEADER;

//         if (cs == c) {
//           memcpy(&d, buf, len);
//           return READ_OK;
//         } else {
//           return READ_CHECKSUM_ERROR;
//         }
//       }
//     }
//   }

//   return READ_NOT_ENOUGH_DATA;
// }

template <typename readT>
ReadResult readPacket(HardwareSerial &seri, readT &d) {
    enum State { WAIT_HEADER, WAIT_HEADER2, WAIT_LEN, WAIT_DATA, WAIT_CHECKSUM };
    static uint8_t buf[sizeof(readT)];
    static uint8_t idx = 0;
    static uint8_t len = 0;
    static State state = WAIT_HEADER;

    ReadResult result = READ_NOT_ENOUGH_DATA;

    if (seri.available() > 64) {   // 64は目安、パケットサイズに応じて調整、溜まりすぎてたらデータ捨ててリセット
      while (seri.available()) seri.read();
      idx = 0;
      state = WAIT_HEADER;
      return READ_NOT_ENOUGH_DATA;  // データ破棄したので次回読み直し
    }

    while (seri.available()) {  // バッファにある全てのデータを処理
        uint8_t c = seri.read();

        switch (state) {
            case WAIT_HEADER:
                if (c == 0xAA) state = WAIT_HEADER2;
                break;

            case WAIT_HEADER2:
                if (c == 0x55) state = WAIT_LEN;
                else state = WAIT_HEADER;
                break;

            case WAIT_LEN:
                if (c == sizeof(readT)) {
                    len = c;
                    idx = 0;
                    state = WAIT_DATA;
                } else state = WAIT_HEADER;
                break;

            case WAIT_DATA:
                buf[idx++] = c;
                if (idx >= len) state = WAIT_CHECKSUM;
                break;

            case WAIT_CHECKSUM: {
                uint8_t cs = len;
                for (uint8_t i = 0; i < len; i++) cs ^= buf[i];

                state = WAIT_HEADER;

                if (cs == c) {
                    memcpy(&d, buf, len);
                    result = READ_OK;  // 最新値がここでセットされる
                }
                break;
            }
        }
    }

    return result;
}


// =============================================================

void initUART();
void UART();