// ========= パケット用データ =========
// struct ReadPacketData0 { int ball_angle; int goal_angle; };
// struct ReadPacketData1 { int ball_angle; int ball_distance; };
// struct ReadPacketData2 { int x; };
#include <Arduino.h>
ReadPacketData0 rp0;
ReadPacketData1 rp1;
ReadPacketData2 rp2;

// struct SendPacketData0 { int ball_angle; int ball_distance; };
// struct SendPacketData1 { int ball_angle; int goal_angle; };
// struct SendPacketData2 { int x; };

SendPacketData0 sp0;
SendPacketData1 sp1;
SendPacketData2 sp2;

// ========= ユーティリティ =========
uint8_t calcChecksum(uint8_t* data, uint8_t len) {
  uint8_t sum = 0;
  for (int i = 0; i < len; i++) sum ^= data[i];
  return sum;
}

template <typename readT>
bool readPacket(int num_serial, readT &d) {

  HardwareSerial *seri;
  if (num_serial == 0)      seri = &Serial;
  else if (num_serial == 1) seri = &Serial1;
  else                      seri = &Serial2;

  uint8_t header;

  // ========= ヘッダー検索（ブロッキングしない） =========
  while (seri->available()) {
    header = seri->read();
    if (header == 0xAA) break;
  }

  // ヘッダが来ていないなら終了
  if (header != 0xAA) return false;

  // ========= 長さ取得 =========
  if (!seri->available()) return false;
  uint8_t len = seri->read();

  // サイズ不一致は即破棄
  if (len != sizeof(readT)) {
    while (seri->available()) seri->read();  // ✅ バッファ全消去
    return false;
  }

  // ========= 受信バッファが多すぎたら古いのを捨てる =========
  int expected = len + 1;  // data + checksum

  if (seri->available() > expected) {
    int trash = seri->available() - expected;
    while (trash--) seri->read();   // ✅ 古いデータ破棄
  }

  // ========= 必要分が揃っていなければ待たずに終了 =========
  if (seri->available() < expected) return false;

  // ========= データ受信 =========
  uint8_t buf[64];
  for (int i = 0; i < len; i++) {
    buf[i] = seri->read();
  }

  uint8_t checksum = seri->read();

  // ========= チェックサム検証 =========
  uint8_t sum = calcChecksum(buf, len);
  if (checksum != sum) return false;

  // ========= struct にコピー =========
  memcpy(&d, buf, len);
  return true;
}


// ========= パケット読み取り =========
// template <typename readT>
// bool readPacket(int num_serial, readT &d) {

//     HardwareSerial *seri;
//     if (num_serial == 0) seri = &Serial;
//     else if (num_serial == 1) seri = &Serial1;
//     else seri = &Serial2;

//     uint32_t start = millis();
//     uint8_t header;

//     // --- ヘッダー検索（タイムアウト 30ms） ---
//     while (true) {
//       if (seri->available()) {
//         header = seri->read();
//         if (header == 0xAA) break;  // 見つかった
//       }
//       if (millis() - start > 30) return false; // タイムアウト
//     }

//     // --- 長さ取得 ---
//     while (!seri->available()) {
//       if (millis() - start > 30) return false;
//     }
//     uint8_t len = seri->read();
//     if (len != sizeof(readT)) return false;

//     // --- データ + チェックサムが来るまで待つ ---
//     while (seri->available() < len + 1) {
//       if (millis() - start > 30) return false;
//     }

//     // --- 読み込み ---
//     uint8_t buf[64];
//     for (int i = 0; i < len; i++) buf[i] = seri->read();
//     uint8_t checksum = seri->read();

//     // --- チェックサム検証 ---
//     uint8_t sum = calcChecksum(buf, len);
//     if (checksum != sum) return false;

//     // --- struct にコピー ---
//     memcpy(&d, buf, len);

//     return true;
// }

// ========= パケット送信 =========
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

// ========= UART 初期化 =========
void initUART() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 18, 19);  //TX, RX // to MAIN_ESP32
  Serial2.begin(115200, SERIAL_8N1, 17, 16);  // to Echo_ArduinoNano
}

// int dis;

int16_t serial_date;

// ========= UART 読み取り =========
void UART() {
  sp0.ball_angle = ball_angle;
  sp0.ball_distance = ball_distance;
  sp0.ball_max = ball_max;
  // sp0.ball_distance = getBallAngleAndDistance().distance;
  readPacket(2, rp2);
  // Serial.println(rp2.dis);
  sp0.dis1 = rp2.dis1;
  sp0.dis2 = rp2.dis2;
  sp0.dis3 = rp2.dis3;
  sp0.dis4 = rp2.dis4;
  
  sendPacket(0, sp0);

  sp2.serial_date = ball_angle;
  sendPacket(2, sp2);
}