#include "functions.h"
#include "IMU.h"
#include "UART.h"

class Line {
  private:
    int16_t last_field_line_angle = 0;
    int16_t last_line_angle = 0;
    static constexpr int16_t AVOID_LINE_SPEED = 500;
    static constexpr int16_t BORDER_OVER_LINE = 130;

    bool over_line = false;

  public:
    int16_t serial_angle;

    int getFieldLineAngle(int16_t line_angle) {
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

    Vector cancelV(int16_t field_line_angle, Vector cancel_v) {
      int16_t len_cancel_v = lenV(cancel_v, field_line_angle + 180);
      if (len_cancel_v > 0) { // field_line_angle側にベクトルがある時は正、ない時は負になる
        return makeV(field_line_angle, len_cancel_v);
      }
      else {
        return {0, 0};
      }
    }

    Vector lineV(int16_t line_angle, Vector cancel_v) {
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
};

class Ball {
  private:
    static constexpr int16_t BALL_SPEED = 500;
    static constexpr uint8_t KdigDis[17] = {0, 0, 0, 20, 20, 20, 30, 30, 30, 30, 60, 90, 90, 90, 90, 90, 90};
  public:
    Vector ballV(int16_t ball_angle, int16_t ball_distance) {
      Vector ball_v;

      if (ball_angle == 400) {
        ball_v = {0, 0};
      }
      else {
        // if (abs(convertAngle(ball_angle)) < 30) {
        //   ball_v = makeV(reverseAngle(convertAngle(ball_angle) * 1.3), BALL_SPEED);
        // }
        // else {
        //   int lr = convertAngle(ball_angle) > 0 ? 1 : -1;
        //   ball_v = makeV(reverseAngle(convertAngle(ball_angle) + KdigDis[ball_distance] * lr), BALL_SPEED);

        // }
        ball_v = makeV(0, convertAngle(ball_angle) * 10);
      }

      return ball_v;
    }
};

class Field {
  private:
    static constexpr int16_t X_MIN_FIELD = 1200;
    static constexpr int16_t Y_MIN_FIELD = 1800;

    int16_t last_position_x;
    int16_t last_position_y;
  public:
    int16_t position_x;
    int16_t position_y;

    void getFeildPosition(int echo_right, int echo_left, int echo_plus, int echo_minus, int move_x, int move_y, int angle) {  // 中央が0,0
      int echo_sum_x = echo_right + echo_left;
      int echo_sum_y = echo_plus + echo_minus;

      if (echo_sum_x >= X_MIN_FIELD) {
        position_x = (echo_sum_x / 2) - echo_right;  // 座標を返す
        last_position_x = position_x;  // 座標を返す
      }
      else {
        position_x = move_x + last_position_x;
      }
    }
};

Line line;
Ball ball;
Field field;

const int8_t beep = 23;

void setup() {
  pinMode(beep, OUTPUT);
  initIMU();
  initUART();
}

Vector main_v;

void loop() {
  IMU();
  UART();

  // field.getFeildPosition(data_hub.echo_2, data_hub.echo_6, data_hub.echo_0, data_hub.echo_4, data_hub.move_x, data_hub.move_y, 0);
  Vector ball_v = ball.ballV(data_sub.ball_angle, data_sub.ball_distance);
  Vector line_v = line.lineV(data_sub.line_angle, ball_v);
  main_v = assembleV(line_v, ball_v, 700);
  if (abs(angleZ > 20)) {
    main_v = {0, 0};
  }
  // main_v = addV(ball_v, line_v);
  Serial.print(data_sub.ball_angle);
  Serial.print(" : ");
  Serial.println(data_sub.ball_distance);
}
