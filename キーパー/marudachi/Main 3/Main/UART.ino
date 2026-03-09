#include <Arduino.h>
#include "UART.h"

// ToHub to_hub;

// FromHub from_hub;
// FromHub data_hub;

// FromSub from_sub;
// FromSub data_sub;

void UART::initUART() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 16, 17);
  Serial2.begin(115200, SERIAL_8N1, 18, 19);
}

void UART::updateUART() {
  if (readPacket(Serial2, from_sub) == READ_OK) {
    // data_sub = from_sub;
    data.dp.ball_angle = from_sub.ball_angle;
    data.dp.ball_distance = from_sub.ball_distance;
    data.dp.line_angle = from_sub.line_angle;
    data.dp.line_distance = from_sub.line_distance;
  }

  if (readPacket(Serial1, from_hub) == READ_OK) {
    // data_hub = from_hub;
    data.dp.vx = from_hub.vx;
    data.dp.vy = from_hub.vy;
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

  

  to_hub.vx = attack.getAttackData().v.x;
  to_hub.vy = attack.getAttackData().v.y;
  to_hub.omega = attack.getAttackData().rotate;

  to_hub.content = from_sub.line_angle;

  sendPacket(Serial1, to_hub);
}

void Data::updateData() {
  dp.robot_angle = gyro.getAngle();
}