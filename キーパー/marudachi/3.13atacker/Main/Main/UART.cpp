#include "Instance.h"
#include "UART.h"

// template <typename sendT>
// void UART::sendPacket(HardwareSerial &seri, const sendT &d) { // *ポインタ、ではなく&参照で安全に
//   // static_assert(condition, "message"); 通信バイトが255バイトを超えるとコンパイルエラーでmessageを表示する
//   static_assert(sizeof(sendT) <= 255, "Packet too large");

//   const uint8_t* p = reinterpret_cast<const uint8_t*>(&d);  // Serial.writeに必要なunit8_t*の型に、reinterpret_castで変換する
//   const uint8_t len = sizeof(sendT);

//   uint8_t cs = len;  // checkSumにid、len、中身のデータをかける
//   for (uint8_t i = 0; i < len; i++) {
//     cs ^= p[i];
//   }

//   seri.write(0xAA); // 参照 、ポインタの場合はseri->write();
//   seri.write(0x55);
//   seri.write(len);
//   seri.write(p, len);
//   seri.write(cs);
// }
 
// template <typename readT>
// ReadResult UART::readPacket(HardwareSerial &seri, readT &d) {
//     enum State { WAIT_HEADER, WAIT_HEADER2, WAIT_LEN, WAIT_DATA, WAIT_CHECKSUM };
//     static uint8_t buf[sizeof(readT)];
//     static uint8_t idx = 0;
//     static uint8_t len = 0;
//     static State state = WAIT_HEADER;

//     ReadResult result = READ_NOT_ENOUGH_DATA;

//     if (seri.available() > 64) {   // 64は目安、パケットサイズに応じて調整、溜まりすぎてたらデータ捨ててリセット
//       while (seri.available()) seri.read();
//       idx = 0;
//       state = WAIT_HEADER;
//       return READ_NOT_ENOUGH_DATA;  // データ破棄したので次回読み直し
//     }

//     while (seri.available()) {  // バッファにある全てのデータを処理
//         uint8_t c = seri.read();

//         switch (state) {
//             case WAIT_HEADER:
//                 if (c == 0xAA) state = WAIT_HEADER2;
//                 break;

//             case WAIT_HEADER2:
//                 if (c == 0x55) state = WAIT_LEN;
//                 else state = WAIT_HEADER;
//                 break;

//             case WAIT_LEN:
//                 if (c == sizeof(readT)) {
//                     len = c;
//                     idx = 0;
//                     state = WAIT_DATA;
//                 } else state = WAIT_HEADER;
//                 break;

//             case WAIT_DATA:
//                 buf[idx++] = c;
//                 if (idx >= len) state = WAIT_CHECKSUM;
//                 break;

//             case WAIT_CHECKSUM: {
//                 uint8_t cs = len;
//                 for (uint8_t i = 0; i < len; i++) cs ^= buf[i];

//                 state = WAIT_HEADER;

//                 if (cs == c) {
//                     memcpy(&d, buf, len);
//                     result = READ_OK;  // 最新値がここでセットされる
//                 }
//                 break;
//             }
//         }
//     }

//     return result;
// }

void UART::initUART() {
  Serial.begin(115200); // シリアルモニター用
  Serial1.begin(115200, SERIAL_8N1, 16, 17);  // シリアルbegin
  Serial2.begin(115200, SERIAL_8N1, 18, 19);
}

void UART::updateUART() {
  if (readPacket(Serial2, from_sub) == UART::READ_OK) {
    // data_sub = from_sub;
    data.dp.ball_angle = from_sub.ball_angle; // dpに通信でゲットした値を代入して、dpから使えるようにする
    data.dp.ball_distance = from_sub.ball_distance;
    data.dp.line_angle = from_sub.line_angle;
    data.dp.line_distance = from_sub.line_distance;
  }

  if (readPacket(Serial1, from_hub) == UART::READ_OK) {
    // data_hub = from_hub;
    data.dp.velo_x = from_hub.vx;
    data.dp.velo_y = from_hub.vy;

    data.dp.velocity_v.x = data.dp.velo_x;
    data.dp.velocity_v.y = data.dp.velo_y;

    data.dp.move_x = from_hub.move_x;
    data.dp.move_y = from_hub.move_y;
    
    data.dp.echo_0 = from_hub.echo_0;
    data.dp.echo_1 = from_hub.echo_1;
    data.dp.echo_2 = from_hub.echo_2;
    data.dp.echo_3 = from_hub.echo_3;
    data.dp.echo_4 = from_hub.echo_4;
    data.dp.echo_5 = from_hub.echo_5;
    data.dp.echo_6 = from_hub.echo_6;
    data.dp.echo_7 = from_hub.echo_7;

    data.dp.id = from_hub.id;
    data.dp.ack = from_hub.ack;
    data.dp.content_id = from_hub.content_id;
    data.dp.content = from_hub.content;
  }

  

  to_hub.vx = attack.getAttackData().v.x; // モーターマイコンへの情報をsendPacketに入れる
  to_hub.vy = attack.getAttackData().v.y;
  to_hub.omega = attack.getAttackData().rotate;
  to_hub.ball_angle = data.dp.ball_angle;
  to_hub.line_angle = data.dp.line_angle;
  to_hub.robot_angle = data.dp.robot_angle;

  
  sendPacket(Serial1, to_hub);  // モーターマイコンへデータの入ったパケットを送信
}

void Data::updateData() {
  dp.robot_angle = gyro.getAngle();
}

// #include <Arduino.h>
// #include "UART.h"

// // ToHub to_hub;

// // FromHub from_hub;
// // FromHub data_hub;

// // FromSub from_sub;
// // FromSub data_sub;

// void UART::initUART() {
//   Serial.begin(115200);
//   Serial1.begin(115200, SERIAL_8N1, 16, 17);
//   Serial2.begin(115200, SERIAL_8N1, 18, 19);
// }

// void UART::updateUART() {
//   if (readPacket(Serial2, from_sub) == READ_OK) {
//     // data_sub = from_sub;
//     data.dp.ball_angle = from_sub.ball_angle;
//     data.dp.ball_distance = from_sub.ball_distance;
//     data.dp.line_angle = from_sub.line_angle;
//     data.dp.line_distance = from_sub.line_distance;
//   }

//   if (readPacket(Serial1, from_hub) == READ_OK) {
//     // data_hub = from_hub;
//     data.dp.vx = from_hub.vx;
//     data.dp.vy = from_hub.vy;
//     data.dp.move_x = from_hub.move_x;
//     data.dp.move_y = from_hub.move_y;
    
//     data.dp.echo_0 = from_hub.echo_0;
//     data.dp.echo_1 = from_hub.echo_1;
//     data.dp.echo_2 = from_hub.echo_2;
//     data.dp.echo_3 = from_hub.echo_3;
//     data.dp.echo_4 = from_hub.echo_4;
//     data.dp.echo_5 = from_hub.echo_5;
//     data.dp.echo_6 = from_hub.echo_6;
//     data.dp.echo_7 = from_hub.echo_7;

//     data.dp.id = from_hub.id;
//     data.dp.ack = from_hub.ack;
//     data.dp.content_id = from_hub.content_id;
//     data.dp.content = from_hub.content;
//   }

  

//   to_hub.vx = attack.getAttackData().v.x;
//   to_hub.vy = attack.getAttackData().v.y;
//   to_hub.omega = attack.getAttackData().rotate;

//   to_hub.content = from_sub.line_angle;

//   sendPacket(Serial1, to_hub);
// }

// void Data::updateData() {
//   dp.robot_angle = gyro.getAngle();
// }