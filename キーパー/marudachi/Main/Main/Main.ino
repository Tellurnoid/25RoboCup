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
  
  public:
    Vector ballV(int16_t ball_angle) {
      Vector ball_v;

      if (ball_angle == 400) {
        ball_v = {0, 0};
      }
      else {
        ball_v = makeV(reverseAngle(ball_angle * 1.5), BALL_SPEED);
      }

      return ball_v;
    }
};

Line line;
Ball ball;

const int8_t beep = 23;

void setup() {
  pinMode(beep, OUTPUT);
  initIMU();
  initUART();
}

Vector main_v;
Vector last_lineV = makeV(0,200);
Vector keeperLineV(){
  float line_angle = data_sub.line_angle;
  float line_dis = data_sub.line_distance;
  Vector v;
  float Kp = 3.0f;
  float Power_linetrace;
  
  if(line_angle == 400){
    v = last_lineV;
  }
  else{
    Power_linetrace = line_dis * Kp;
    v = makeV(reverseAngle(line_angle),Power_linetrace);
    last_lineV = v; 
  }

      //白線の中央にいる動き
  return v;
}
Vector straightDebugV(){
  Vector v;
  if(data_sub.line_angle == 400){
    v = makeV(reverseAngle(0), 700);
    return v;
  }
  else{
    v = makeV(reverseAngle(0), 0);
    return v;
  }
}

void loop() {
  IMU();
  UART();
  // delay(5);
  Vector ball_v = ball.ballV(data_sub.ball_angle);
  Vector line_v = line.lineV(data_sub.line_angle, ball_v);
  //main_v = addV(ball_v, line_v);
  //main_v = keeperLineV();
  main_v = straightDebugV();
}
