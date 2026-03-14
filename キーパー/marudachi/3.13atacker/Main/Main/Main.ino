#include "functions.h"
#include "Instance.h"
#include "Main.h"
#include "IMU.h"
#include "UART.h"

void Line::updataLine() {
  line_angle = data.dp.line_angle;
  line_distance = data.dp.line_distance;
}

int Line::getFieldLineAngle() {
  int16_t field_line_angle;
  if (line_angle == 400) {
    field_line_angle = over_line == true ? last_field_line_angle : 400; // もしラインを超えてたら、戻る
  }
  else if (line_angle == 500) {
    field_line_angle = last_field_line_angle; // last_field_line_angleが400の時は諦め
  }
  else {
    if (last_line_angle != 400 && last_line_angle != 500 && abs(angleDiff(line_angle, last_line_angle)) > BORDER_OVER_LINE) {  // もし中央を跨いだら
      over_line = over_line == true ? false : true; // 前との逆の値を入れる
    }

    field_line_angle = over_line == true ? line_angle : line_angle + 180; // field_lineは戻る方向
    field_line_angle = wrap180(field_line_angle);
  }

  last_line_angle = line_angle;
  last_field_line_angle = field_line_angle;


  serial_angle = field_line_angle;  //////////////////////////////
  return field_line_angle;
}

int Line::getFieldLineDistance() {
  int16_t dis = over_line == true ? -line_distance : line_distance;
  return dis;
}

Vector Line::cancelV(int16_t field_line_angle, Vector cancel_v) {
  int16_t len_cancel_v = lenV(cancel_v, field_line_angle + 180);
  // Serial.println(len_cancel_v);
  if (len_cancel_v > 0) { // field_line_angle側にベクトルがある時は正、ない時は負になる
    return makeV(field_line_angle, len_cancel_v);
  }
  else {
    return {0, 0};
  }
}

Vector Line::lineV(int16_t line_angle, Vector cancel_v) {
  int16_t field_line_angle = getFieldLineAngle();

  if (field_line_angle == 400) {
    return {0, 0};
  }
  else {
    float line_speed = lenV(data.dp.velocity_v, reverseAngle(field_line_angle) + 180);
    // line_speed *= AVLID_LINE_Kp;
    // if (line_speed < AVLID_LINE_MIN_SPEED) line_speed = AVLID_LINE_MIN_SPEED;
    float control_keep_line = (KEEP_LINE_DIS - getFieldLineDistance()) * KEEP_LINE_Kp;
    if (line_angle == 400 || line_angle == 500) control_keep_line = 100 * KEEP_LINE_Kp;
    // if (line_speed > 100) control_keep_line = -AVOID_LINE_SPEED;

    Vector line_v = makeV(reverseAngle(field_line_angle), control_keep_line);
    // Vector line_v = {0, 0};

    // Vector line_v = makeV(reverseAngle(field_line_angle), line_speed);
    // Vector line_v = makeV(reverseAngle(field_line_angle), AVOID_LINE_SPEED);
    // Vector canceled_v = cancelV(reverseAngle(field_line_angle), cancel_v);
    // return addV(line_v, canceled_v);
    return line_v;
  }
}

Vector Ball::ballV(int16_t ball_angle, int16_t ball_distance) {
  Vector ball_v;

  if (ball_angle == 400) {
    ball_v = {0, 0};
  }
  else {
    int front_angle = 45;
    if (abs(convertAngle(ball_angle)) < front_angle) {
      // ball_v = makeV(reverseAngle(convertAngle(ball_angle) * 1.3), BALL_SPEED);
      Vector ball_vx = makeV(0, convertAngle(ball_angle) * 15);
      Vector ball_vy = makeV(90, abs(front_angle - convertAngle(ball_angle)) * 30);
      ball_v = addV(ball_vx, ball_vy);
    }
    else {
      int lr = convertAngle(ball_angle) > 0 ? 1 : -1;
      ball_v = makeV(reverseAngle(convertAngle(ball_angle) + KdigDis[ball_distance] * lr), BALL_SPEED);
    }
    // ball_v = makeV(0, convertAngle(ball_angle) * 10);
  }

  return ball_v;
}

void Attack::initAttack() {
  gyro.initGyro();
}

void Attack::setBehavior(Behavior b) {
  behavior = b;
}

void Attack::updateAttack() {
  gyro.updateGyro();
  line.updataLine();

  if (data.dp.id == MAIN && data.dp.ack != ack) {
    int content = data.dp.content;
    switch (data.dp.content_id) {
    case START:
      if (content == 0) setBehavior(OFF);
      if (content == 1) setBehavior(BALL_CHASE);
      break;
    }
  }

  switch (behavior) {
    case BALL_CHASE:
      ballChase();
      break;

    case SHOOT:
      shoot();
      break;

    case STOP:
      stop();
      break;

    case OFF:
      off();
      break;
  }
}

void Attack::ballChase() {
  Vector ball_v = ball.ballV(data.dp.ball_angle, data.dp.ball_distance);

  int field_line_angle = line.getFieldLineAngle();

  Vector ball_cancel_v = makeV(reverseAngle(field_line_angle),lenV(ball_v, reverseAngle(field_line_angle) + 180));


  Serial.print(" :cx: ");
  Serial.print(ball_cancel_v.x);
  Serial.print(" :cy: ");
  Serial.print(ball_cancel_v.y);

  if (field_line_angle != 400)  ball_v = addV(ball_v, ball_cancel_v);
  // Vector ball_cancel_v = makeV(reverseAngle(line.getFieldLineAngle()),lenV(ball_v, reverseAngle(line.getFieldLineAngle()) + 180));


  Serial.print(" :x: ");
  Serial.print(ball_v.x);
  Serial.print(" :y: ");
  Serial.print(ball_v.y);

  Vector line_v = line.lineV(data.dp.line_angle, ball_v); 


  Serial.print(" :lx: ");
  Serial.print(line_v.x);
  Serial.print(" :ly: ");
  Serial.print(line_v.y);
  Serial.print(" :line_angle: ");
  Serial.print(data.dp.line_angle);
  Serial.print(" :field_line_angle: ");
  Serial.println(line.getFieldLineAngle());
  // if (line.getFieldLineAngle() != 400)  ball_v = addV(ball_v, ball_cancel_v);
  attack_v = assembleV(line_v, ball_v, MAX_SPEED);

  attack_rotate = gyro.controlAngle(0);
}

void Attack::shoot() {

}

void Attack::stop() {
  attack_v = {0, 0};
  attack_rotate = gyro.controlAngle(0);
}

void Attack::off() {
  attack_v = {0, 0};
  attack_rotate = 0;
}


const int8_t beep = 23;

void setup() {
  pinMode(beep, OUTPUT);
  // gyro.initGyro();
  attack.initAttack();
  uart.initUART();
}

void loop() {
  uart.updateUART(); // 受信
  data.updateData();

  // attack.setBehavior(Attack::BALL_CHASE);
  attack.updateAttack();

  uart.updateUART(); // 送信

  // Serial.print(data.dp.velo_x);
  // Serial.print(" : ");
  // Serial.println(data.dp.velo_y);
}