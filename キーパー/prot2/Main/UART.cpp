#include "Instance.h"
#include "UART.h"

SerialPIO PIOSerial1(20, 21); // Motor
SerialPIO PIOSerial2(8, 9); // UI
SerialPIO PIOSerial3(1, 0); // Sub
SerialPIO PIOSerial4(11, 22); // Camera

void UART::init() {
  Serial.begin(115200); // シリアルモニター用

  PIOSerial1.begin(115200);
  PIOSerial2.begin(115200);
  PIOSerial3.begin(115200);
  PIOSerial4.begin(115200);
}

void UART::update() {
  if (readPacket(PIOSerial1, from_motor) == UART::READ_OK) {
    data.dp.velo_x = from_motor.vx;
    data.dp.velo_y = from_motor.vy;

    data.dp.velocity_v.x = data.dp.velo_x;
    data.dp.velocity_v.y = data.dp.velo_y;

    data.dp.move_x = (float)from_motor.move_x;
    data.dp.move_y = (float)from_motor.move_y;
  }
   
  if (readPacket(PIOSerial3, from_sub) == UART::READ_OK) {
    // data_sub = from_sub;
    data.dp.ball_angle = from_sub.ball_angle; // dpに通信でゲットした値を代入して、dpから使えるようにする
    data.dp.ball_distance = from_sub.ball_distance;
    data.dp.line_angle = from_sub.line_angle;
    data.dp.line_distance = from_sub.line_distance;
  }

  if (readPacket(PIOSerial2, from_ui) == UART::READ_OK) {
    data.dp.echoValues[0] = from_ui.echo_0;
    data.dp.echoValues[1] = from_ui.echo_1;
    data.dp.echoValues[2] = from_ui.echo_2;
    data.dp.echoValues[3] = from_ui.echo_3;
    data.dp.echoValues[4] = from_ui.echo_4;
    data.dp.echoValues[5] = from_ui.echo_5;
    data.dp.echoValues[6] = from_ui.echo_6;
    data.dp.echoValues[7] = from_ui.echo_7;

    data.dp.id = from_ui.id;
    data.dp.ack = from_ui.ack;
    data.dp.content_id = from_ui.content_id;
    data.dp.content = from_ui.content;
  }

  if (readPacket(PIOSerial4, from_camera) == UART::READ_OK) {
    data.dp.goal_angle = from_camera.angle;
    //Serial.println(data.dp.goal_angle);
  }


  to_motor.vx = data.dp.main_v.x; // モーターマイコンへの情報をsendPacketに入れる
  to_motor.vy = data.dp.main_v.y;
  to_motor.omega = data.dp.main_rotate;    
  
  sendPacket(PIOSerial1, to_motor);  // モーターマイコンへデータの入ったパケットを送信


  to_ui.ball_angle = data.dp.ball_angle;
  to_ui.line_angle = data.dp.line_angle;
  to_ui.robot_angle = data.dp.robot_angle;
  to_ui.goal_angle = data.dp.goal_angle;

  sendPacket(PIOSerial2, to_ui);


  to_sub.id = from_ui.id;
  to_sub.ack = from_ui.ack;
  to_sub.content_id = from_ui.content_id;
  to_sub.content = from_ui.content;

  sendPacket(PIOSerial3, to_sub);
}

void Data::init() {
  gyro.init();
}

void Data::update() {
  gyro.update();
  dp.robot_angle = gyro.getAngle();
}