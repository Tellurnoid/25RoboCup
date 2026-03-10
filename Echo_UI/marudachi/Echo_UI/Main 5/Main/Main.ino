#include "functions.h"
#include "Instance.h"
#include "Main.h"
#include "IMU.h"
#include "UART.h"

int Line::getFieldLineAngle(int16_t line_angle) {
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

    field_line_angle = over_line == true ? line_angle : line_angle + 180;
    field_line_angle = wrap180(field_line_angle);
  }

  last_line_angle = line_angle;
  last_field_line_angle = field_line_angle;


  serial_angle = field_line_angle;  //////////////////////////////
  return field_line_angle;
}

Vector Line::cancelV(int16_t field_line_angle, Vector cancel_v) {
  int16_t len_cancel_v = lenV(cancel_v, field_line_angle + 180);
  if (len_cancel_v > 0) { // field_line_angle側にベクトルがある時は正、ない時は負になる
    return makeV(field_line_angle, len_cancel_v);
  }
  else {
    return {0, 0};
  }
}

Vector Line::lineV(int16_t line_angle, Vector cancel_v) {
  int16_t field_line_angle = getFieldLineAngle(line_angle);
  if (field_line_angle == 400) {
    return {0, 0};
  }
  else {
    Vector line_v = makeV(reverseAngle(field_line_angle), AVOID_LINE_SPEED);
    Vector canceled_v = cancelV(reverseAngle(field_line_angle), cancel_v);
    return addV(line_v, canceled_v);
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

// Gyro gyro;
// Ball ball;
// Line line;
// Data data;

// UART uart;

void Attack::initAttack() {
  gyro.initGyro();
}

void Attack::setBehavior(Behavior b) {
  behavior = b;
}

void Attack::updateAttack() {
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
  Vector line_v = line.lineV(data.dp.line_angle, ball_v); 
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
  gyro.initGyro();
  attack.initAttack();
  uart.initUART();
}

void loop() {
  gyro.updateGyro();

  uart.updateUART(); // 受信
  data.updateData();

  attack.setBehavior(Attack::BALL_CHASE);
  attack.updateAttack();

  uart.updateUART(); // 送信

  Serial.println(data.dp.line_angle);
}
// #include "functions.h"
// #include "Instance.h"
// #include "IMU.h"
// #include "UART.h"

// class Line {
//   private:
//     int16_t last_field_line_angle = 0;
//     int16_t last_line_angle = 0;
//     static constexpr int16_t AVOID_LINE_SPEED = 500;
//     static constexpr int16_t BORDER_OVER_LINE = 130;

//     bool over_line = false;

//   public:
//     int16_t serial_angle;

//     int getFieldLineAngle(int16_t line_angle) {
//       int16_t field_line_angle;
//       if (line_angle == 400) {
//         field_line_angle = over_line == true ? last_field_line_angle : 400; // もしラインを超えてたら、戻る
//       }
//       else if (line_angle == 500) {
//         field_line_angle = last_field_line_angle; // last_field_line_angleが400の時は諦め
//       }
//       else {
//         if (last_line_angle != 400 && last_line_angle != 500 && abs(angleDiff(line_angle, last_line_angle)) > BORDER_OVER_LINE) {  // もし中央を跨いだら
//           over_line = over_line == true ? false : true; // 前との逆の値を入れる
//         }

//         field_line_angle = over_line == true ? line_angle : line_angle + 180;
//         field_line_angle = wrap180(field_line_angle);
//       }

//       last_line_angle = line_angle;
//       last_field_line_angle = field_line_angle;


//       serial_angle = field_line_angle;  //////////////////////////////
//       return field_line_angle;
//     }

//     Vector cancelV(int16_t field_line_angle, Vector cancel_v) {
//       int16_t len_cancel_v = lenV(cancel_v, field_line_angle + 180);
//       if (len_cancel_v > 0) { // field_line_angle側にベクトルがある時は正、ない時は負になる
//         return makeV(field_line_angle, len_cancel_v);
//       }
//       else {
//         return {0, 0};
//       }
//     }

//     Vector lineV(int16_t line_angle, Vector cancel_v) {
//       int16_t field_line_angle = getFieldLineAngle(line_angle);
//       if (field_line_angle == 400) {
//         return {0, 0};
//       }
//       else {
//         Vector line_v = makeV(reverseAngle(field_line_angle), AVOID_LINE_SPEED);
//         Vector canceled_v = cancelV(reverseAngle(field_line_angle), cancel_v);
//         return addV(line_v, canceled_v);


//         // Vector line_v = makeV(reverseAngle(field_line_angle), line_distance * 3);
//         // Vector canceled_v = cancelV(reverseAngle(field_line_angle), cancel_v);
//         // return addV(line_v, canceled_v);
//       }
//     }
// };

// class Ball {
//   private:
//     static constexpr int16_t BALL_SPEED = 500;
//     static constexpr uint8_t KdigDis[17] = {0, 0, 0, 20, 20, 20, 30, 60, 60, 90, 90, 90, 90, 90, 90, 90, 90};
//   public:
//     Vector ballV(int16_t ball_angle, int16_t ball_distance) {
//       Vector ball_v;

//       if (ball_angle == 400) {
//         ball_v = {0, 0};
//       }
//       else {
//         int front_angle = 45;
//         if (abs(convertAngle(ball_angle)) < front_angle) {
//           // ball_v = makeV(reverseAngle(convertAngle(ball_angle) * 1.3), BALL_SPEED);
//           Vector ball_vx = makeV(0, convertAngle(ball_angle) * 15);
//           Vector ball_vy = makeV(90, abs(front_angle - convertAngle(ball_angle)) * 30);
//           ball_v = addV(ball_vx, ball_vy);
//         }
//         else {
//           int lr = convertAngle(ball_angle) > 0 ? 1 : -1;
//           ball_v = makeV(reverseAngle(convertAngle(ball_angle) + KdigDis[ball_distance] * lr), BALL_SPEED);

//         }
//         // ball_v = makeV(0, convertAngle(ball_angle) * 10);
//       }

//       return ball_v;
//     }
// };

// class Field {
//   private:
//     static constexpr int16_t X_MIN_FIELD = 1200;
//     static constexpr int16_t Y_MIN_FIELD = 1800;

//     int16_t last_position_x;
//     int16_t last_position_y;
//   public:
//     int16_t position_x;
//     int16_t position_y;

//     void getFeildPosition(int echo_right, int echo_left, int echo_plus, int echo_minus, int move_x, int move_y, int angle) {  // 中央が0,0
//       int echo_sum_x = echo_right + echo_left;
//       int echo_sum_y = echo_plus + echo_minus;

//       if (echo_sum_x >= X_MIN_FIELD) {
//         position_x = (echo_sum_x / 2) - echo_right;  // 座標を返す
//         last_position_x = position_x;  // 座標を返す
//       }
//       else {
//         position_x = move_x + last_position_x;
//       }
//     }
// };

// // Gyro gyro;
// // Ball ball;
// // Line line;
// // Data data;

// // UART uart;

// class Attack {
//   public:
//     enum Behavior {
//       BALL_CHASE = 0,
//       SHOOT,
//       STOP,
//       OFF
//     };

//   private:
//     // Gyro my_gyro;
//     // Ball my_ball;
//     // Line my_line;

//     Behavior behavior = OFF;

//     Vector attack_v;
//     int attack_rotate;

//     struct AttackData {
//       Vector v;
//       int rotate;
//     };

//   public:
//     void initAttack() {
//       gyro.initGyro();
//     }

//     void setBehavior(Behavior b) {
//       behavior = b;
//     }

//     void updateAttack() {
//       switch (behavior) {
//         case BALL_CHASE:
//           ballChase();
//           break;

//         case SHOOT:
//           shoot();
//           break;

//         case STOP:
//           stop();
//           break;

//         case OFF:
//           off();
//           break;
//       }
//     }

//     AttackData getAttackData() {
//       return {attack_v, attack_rotate};
//     }

//     // AttackData main_data;

//   private:
//     static constexpr uint16_t MAX_SPEED = 700;

//     void ballChase() {
//       Vector ball_v = ball.ballV(data.dp.ball_angle, data.dp.ball_distance);
//       Vector line_v = line.lineV(data.dp.line_angle, ball_v); 
//       attack_v = assembleV(line_v, ball_v, MAX_SPEED);

//       attack_rotate = gyro.controlAngle(0);
//     }

//     void shoot() {

//     }

//     void stop() {
//       attack_v = {0, 0};
//       attack_rotate = gyro.controlAngle(0);
//     }

//     void off() {
//       attack_v = {0, 0};
//       attack_rotate = 0;
//     }
// };

// // Attack attack;

// const int8_t beep = 23;

// void setup() {
//   pinMode(beep, OUTPUT);
//   attack.initAttack();
//   uart.initUART();
// }

// void loop() {
//   uart.updateUART(); // 受信
//   data.updateData();

//   attack.setBehavior(Attack::BALL_CHASE);
//   attack.updateAttack();

//   uart.updateUART(); // 送信

//   Serial.println(data.dp.line_angle);
// }
