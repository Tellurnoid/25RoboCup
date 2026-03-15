#include <Arduino.h>
#include "Instance.h"
#include "UART.h"

SerialPIO PIOSerial1(20, 21); // Main
SerialPIO PIOSerial2(8, 9); // UI
SerialPIO PIOSerial3(1, 0); // Sub

void UART::initUART() {
  Serial.begin(115200); // シリアルモニター用

  PIOSerial1.begin(115200);
  PIOSerial2.begin(115200);
  PIOSerial3.begin(115200);
}

void UART::updateUART() {
  if (readPacket(PIOSerial3, from_sub) == UART::READ_OK) {
    // data_sub = from_sub;
    data.dp.ball_angle = from_sub.ball_angle; // dpに通信でゲットした値を代入して、dpから使えるようにする
    data.dp.ball_distance = from_sub.ball_distance;
    data.dp.line_angle = from_sub.line_angle;
    data.dp.line_distance = from_sub.line_distance;
  }

  if (readPacket(PIOSerial2, from_ui) == UART::READ_OK) {
    // data_hub = from_hub;
    // data.dp.velo_x = from_hub.vx;
    // data.dp.velo_y = from_hub.vy;

    // data.dp.velocity_v.x = data.dp.velo_x;
    // data.dp.velocity_v.y = data.dp.velo_y;

    // data.dp.move_x = from_hub.move_x;
    // data.dp.move_y = from_hub.move_y;
    
    data.dp.echo_0 = from_ui.echo_0;
    data.dp.echo_1 = from_ui.echo_1;
    data.dp.echo_2 = from_ui.echo_2;
    data.dp.echo_3 = from_ui.echo_3;
    data.dp.echo_4 = from_ui.echo_4;
    data.dp.echo_5 = from_ui.echo_5;
    data.dp.echo_6 = from_ui.echo_6;
    data.dp.echo_7 = from_ui.echo_7;

    Serial.println(data.dp.echo_0);

    data.dp.id = from_ui.id;
    data.dp.ack = from_ui.ack;
    data.dp.content_id = from_ui.content_id;
    data.dp.content = from_ui.content;
  }

  

  to_motor.vx = attack.getAttackData().v.x; // モーターマイコンへの情報をsendPacketに入れる
  to_motor.vy = attack.getAttackData().v.y;
  to_motor.omega = attack.getAttackData().rotate;
  
  sendPacket(PIOSerial1, to_motor);  // モーターマイコンへデータの入ったパケットを送信

  to_ui.ball_angle = data.dp.ball_angle;
  to_ui.line_angle = data.dp.line_angle;
  to_ui.robot_angle = data.dp.robot_angle;

  sendPacket(PIOSerial2, to_ui);
}

void Data::initData() {
  gyro.initGyro();
}

void Data::updateData() {
  gyro.updateGyro();
  dp.robot_angle = gyro.getAngle();
}