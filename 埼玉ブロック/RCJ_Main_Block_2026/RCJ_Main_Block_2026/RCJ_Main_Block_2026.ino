#include "UART.h"
#include "IMU.h"
#include "functions.h"

int angleDiff(int a, int b) {
  int d = a - b;
  while (d > 180) d -= 360;
  while (d < -180) d += 360;
  return d;
}

float getD(Derivative &d, float value, int time) {
  unsigned long now = millis();
  unsigned long dt = now - d.last_time;

  if (d.last_time == 0) { //最初の微分が異様にデカくなるのを防ぐ
    d.last_time = now;
    d.last_value = value;
    return 0;
  }

  if (dt >= time) {
    float diff = value - d.last_value;

    d.value_d = diff / (dt * 0.001f);
    // d.value_d = 0.7 * d.value_d + 0.3 * (diff / (dt * 0.001f));// =(dt / 1000), 移動平均によるノイズ平滑
    // Serial.println(d.value_d);
    
    d.last_value = value;
    d.last_time = now;
  }
  return d.value_d;
}


const int beep = 21;

void setup() {
  initIMU();

  pinMode(beep, OUTPUT);
  initUART();
}

const int FRONT_RANGE = 30;
int target_angle = 0;

Vector ballV() {
  Vector v = {0, 0};
  if (ball_angle == 400) {
    v = makeV(0, 0);
  }
  else {
    if (abs(ball_angle + target_angle) < FRONT_RANGE) {
      v = makeV(0, 150);
    }
    else {
      v = makeV(ball_angle * 1.5, 150);
    }
  }

  return v;
}


int line_start = 400;
int back_angle = 400;
int last_angle = 400;
int crossed = 0;  // 0:まだ反転してない

int last_line = 400;
int in_line = 0;
int over_line = 1;

int stop_last_line = 400;

int final_line_angle;

int serial;

Vector lineV() {
  if (line_angle != 400) {
    line_angle = line_angle * 0.7 + last_angle * 0.3;

    if (line_start == 400) {
      line_start = line_angle;
    }
    int diff = angleDiff(line_angle, line_start);

    // if (line_start > 180) line_start -= 360;
    // if (line_start < -180) line_start += 360;
    int x;
    stop_last_line = x;
    // if (cx != 400) {
      // if (abs(line_angle - cx) > 90) { // angleDiffは-180~180、
      //   x = line_angle + 180;
      // }
      // else {
      //   x = line_angle;
      // }
  
    // else {
      // if (abs(diff) > 20 && abs(diff) < 90) {  /////////////////////////////////////////////////////
      //   line_start = line_angle;
      // }

      if (abs(line_angle - line_start) >= 110) { // 110
        x = line_angle;        // 半周以上 → そのまま
        last_line = x;
        in_line = 1;

        // over_line_dis = -1;
        
      } 
      else {
        x = line_angle + 180;  // 反対方向へ逃げる
        in_line = 0;

        // last_line = x;
        // over_line_dis = 1;
      }

      // if (abs(line_angle - last_angle) > 120) {
      //   if (!crossed) {
      //     over_line *= -1;   // ← 1回だけ
      //     crossed = 1;
      //   }
      // } 
      // else {
      //   crossed = 0;         // ← 条件外に出たらリセット
      // }
      // if (over_line == -1) {
      //   x = line_angle;
      //   last_line = x;
      //   in_line = 1;
      // }
      // else {
      //   x = line_angle + 180;
      //   in_line = 0;
      // }
    // }


    // 角度を正規化（fromPolar対策）
    if (x > 180) x -= 360;
    if (x < -180) x += 360;

    final_line_angle = -x;

    serial = x;

    // Serial.print(x);
    // if (back_angle != line_angle) {
    //   last_angle = back_angle;
    //   back_angle = line_angle;
    // }
    last_angle = line_angle;
    return makeV(x, 400);
    // return makeV(-x, 80 + (40 - line_dis * over_line_dis) * 0.5);
  }
  else {
    final_line_angle = 400;
    
    if (in_line == 1) {
      return makeV(last_line, 500); // ############################################################
    }
    else {
      over_line = 1;  //////////////////////////////////////////////////////
      line_start = 400;
      return makeV(0, 0);
    }
    // line_start = 400;
    // return makeV(stop_last_line, 150);
  }
}


Vector main_v = {0, 0};

void loop() {
  IMU();
  UART();
  main_v = ballV();
  main_v = addV(main_v, lineV());
}
