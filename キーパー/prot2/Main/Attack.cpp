#include "Instance.h"
#include "Attack.h"

Ball ball;
Line line;
Field field;

void Line::update() {
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

void Ball::update() {
  ball_angle = data.dp.ball_angle;
  ball_distance = data.dp.ball_distance;
}

float Ball::getBallD() {
  uint32_t now = millis();
  uint32_t dt_ms = now - last_time;

  if (dt_ms > BALL_D_INTERVAL) {

    float dt = dt_ms / 1000.0f;
    ball_d = (ball_angle - last_ball_angle) / dt;

    last_time = now;
    last_ball_angle = ball_angle;
  }

  return ball_d;
}

Vector Ball::ballV() {
  Vector ball_v;

  if (ball_angle == 400) {
    ball_v = {0, 0};
  }
  else {
    int front_angle = borderDis[ball_distance];
    // int front_angle = 45;
    if (abs(convertAngle(ball_angle)) < front_angle) {
      int error = convertAngle(ball_angle);

      float error_d = getBallD();
      if (error_d < 80) error_d = 0.0f;

      float ball_v_x = error * Kp - error_d * Kd;
      float ball_v_y = (front_angle - abs(error)) * Kp;
      // float ball_v_y = 400;

      ball_v_x *= BALL_SPEED / 780.0f;
      ball_v_y *= BALL_SPEED / 780.0f;
      ball_v = {ball_v_x, ball_v_y};
    }
    else {
      int lr = convertAngle(ball_angle) > 0 ? 1 : -1;
      ball_v = makeV(reverseAngle(convertAngle(ball_angle) + KdigDis[ball_distance] * lr), BALL_SPEED);
    }
    // ball_v = makeV(0, convertAngle(ball_angle) * 10);
  }

  return ball_v;
}

void Field::update() {
  for(int i = 0; i < NUM_ECHOS; i++){
    echoValues[i] = data.dp.echoValues[i];
  }

  move_x = data.dp.move_x;
  move_y = data.dp.move_y;

  robot_angle = data.dp.robot_angle;

  getCrimp();
}

int Field::getEchoForwardIndex() {
  float angle = wrap360(robot_angle) + 22.5;  // 22.5ずらす
  int a = angle / 45; // 0 ~ 8
  a = a % NUM_ECHOS;  // 0 ~ 7にする
  return a;
}

void Field::getCrimp() {
  int echoDiff[NUM_ECHOS];
  for (int i = 0; i < NUM_ECHOS; i++) {
    echoDiff[i] = echoValues[i] - lastEchoValues[i];
    int diff_diff = echoDiff[i] - lastEchoDiff[i];
    crimp[i] = abs(diff_diff) > CRIMP_BORDER ? echoValues[i] : -1;

    lastEchoValues[i] = echoValues[i];
    lastEchoDiff[i] = echoDiff[i];

    Serial.print(":");
    Serial.print(crimp[i]);
  }
  Serial.println();
}



void Attack::init() {
  // gyro.initGyro();
}

void Attack::setBehavior(Behavior b) {
  behavior = b;
}

void Attack::update() {
  line.update();
  ball.update();
  field.update();

  switch (behavior) {
    case BALL_CHASE:
      ballChase();
      data.dp.main_v = attack_v;
      data.dp.main_rotate = attack_rotate;
      break;

    case SHOOT:
      shoot();
      data.dp.main_v = attack_v;
      data.dp.main_rotate = attack_rotate;
      break;

    case STOP:
      stop();
      data.dp.main_v = attack_v;
      data.dp.main_rotate = attack_rotate;
      break;

    case OFF:
      off();
      break;
  }
}

void Attack::ballChase() {
  Vector ball_v = ball.ballV();

  int field_line_angle = line.getFieldLineAngle();

  // if (field_line_angle != 400)  ball_v = addV(ball_v, ball_cancel_v);
  // Vector ball_cancel_v = makeV(reverseAngle(line.getFieldLineAngle()),lenV(ball_v, reverseAngle(line.getFieldLineAngle()) + 180));

  Vector line_v = line.lineV(data.dp.line_angle, ball_v); 
  // if (line.getFieldLineAngle() != 400)  ball_v = addV(ball_v, ball_cancel_v);


  // attack_v = assembleV(line_v, ball_v, MAX_SPEED);
  attack_v = ball_v;

  attack_rotate = gyro.controlAngle(0);
}

void Attack::shoot() {

}

void Attack::stop() {
  attack_v = {0, 0};
  attack_rotate = gyro.controlAngle(0);
}

void Attack::off() {
  // attack_v = {0, 0};
  // attack_rotate = 0;
}