
#include "UART.h"
#include "IMU.h"
#include "functions.h"

// struct Vector {
//   float x;
//   float y;
// };

// Vector makeV(float deg, float speed) {  //角度と速さからベクトルを作る
//   Vector v;
//   float rad = deg * M_PI / 180.0;
//   v.x = cos(rad) * speed;
//   v.y = sin(rad) * speed;
//   return v;
// }

// Vector addV(Vector a, Vector b) {  //ベクトルの足し算
//   Vector result;
//   result.x = a.x + b.x;
//   result.y = a.y + b.y;
//   return result;
// }

// Vector normalizeV(Vector v) { // 正規化（単位ベクトル化）
//   float len = sqrt(v.x * v.x + v.y * v.y);
//   if (len == 0) return {0, 0};
//   return { v.x / len, v.y / len };
// }

// Vector lenV(Vector v, int len_v) {  // 正規化（単位ベクトル化）
//   float len = sqrt(v.x * v.x + v.y * v.y);
//   if (len == 0) return {0, 0};
//   return { v.x / len * len_v, v.y / len * len_v };
// }

// struct Derivative {
//   unsigned long last_time = 0;
//   float last_value = 0;
//   float value_d = 0;
// };

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

const int ball_get_sensor = 34;

void setup() {
  initUART();
  initIMU();
  initMotor();

  pinMode(ball_get_sensor, INPUT);
}

const int NUM_SENSORS = 13;
// const float KdigDis[NUM_SENSORS] = {0, 0, 0, 0, 0.2, 0.4, 0.6, 1, 1, 1, 1, 1, 1}; // 0,1,2,3,...
const float KdigDis[NUM_SENSORS] = {0, 0, 0, 0, 0, 15, 30, 30, 45, 45, 60, 90, 90}; // 0,1,2,3,...

const int FRONT_RANGE = 40;
const int Kp_ball = 1;
const int Kd_ball = 2.7;
const int D_MIN = 300;

const int SPEED = 250;  // 普通のボールチェイス時
const int x_max_speed = 250;  // FRONT_RANG内の時
const int y_max_speed = 250;  //

Derivative ballD;

Vector ballV() {
  Vector v;

  float ball_d = getD(ballD, ball_angle, 30);
  if (abs(ball_d) < D_MIN)  ball_d = 0;

  if (ball_angle == 400) {
    v = makeV(0, 0);
  }
  else {
    if (abs(ball_angle) < FRONT_RANGE) {
      Vector xv = makeV(90, ball_angle * x_max_speed / FRONT_RANGE + ball_d * Kd_ball);

      Vector yv;
      if (abs(ball_angle) < 5) {
        yv = makeV(0, (FRONT_RANGE - abs(ball_angle)) * y_max_speed / FRONT_RANGE);
      }
      else {
        yv = makeV(0, (FRONT_RANGE - abs(ball_angle)) * y_max_speed / FRONT_RANGE);
      }
      // Vector yv = makeV(0, 200);
      v = addV(xv, yv);
    }
    else {
      int lr;
      if (ball_angle > 0) lr = 1;
      else  lr = -1;

      int x = ball_angle + lr * KdigDis[ball_dis];
      v = makeV(x, SPEED);
    }
  }
  //   Vector xv = makeV(0, 0);
  //   if (abs(ball_angle) < FRONT_RANGE) {
  //     xv = makeV(90, ball_d * Kd_ball);

  //     // Vector yv;
  //     // if (abs(ball_angle) < 5) {
  //     //   yv = makeV(0, (FRONT_RANGE - abs(ball_angle)) * y_max_speed / FRONT_RANGE);
  //     // }
  //     // else {
  //     //   yv = makeV(0, (FRONT_RANGE - abs(ball_angle)) * y_max_speed / FRONT_RANGE);
  //     // }
  //     // // Vector yv = makeV(0, 200);
  //     // v = addV(xv, yv);
  //   }
  //   else {
      
  //   }
  //   int lr;
  //   if (ball_angle > 0) lr = 1;
  //   else  lr = -1;

  //   int x = ball_angle + lr * KdigDis[ball_dis];
  //   v = makeV(x, SPEED);
  //   v = addV(v, xv);

  // }
  return v;
}

int line_start = 400;
int last_line = 400;
int in_line = 0;

Vector lineV() {
  if (line_angle != 400) {
    if (line_start == 400) {
      line_start = line_angle;
    }
    int diff = angleDiff(line_angle, line_start);

    // if (line_start > 180) line_start -= 360;
    // if (line_start < -180) line_start += 360;
    int x;
    if (abs(diff) > 110) {
      x = line_angle;        // 半周以上 → そのまま
      last_line = x;
      in_line = 1;
    } 
    else {
      x = line_angle + 180;  // 反対方向へ逃げる
      in_line = 0;
    }
    // 角度を正規化（fromPolar対策）
    if (x > 180) x -= 360;
    if (x < -180) x += 360;

    Serial.print(x);

    return makeV(x, 400);
  }
  else {
    if (in_line == 1) {
      return makeV(last_line, 500); // ############################################################
    }
    else {
      line_start = 400;
      return makeV(0, 0);
    }
  }
  
}

int camera() {
  unsigned long c_now = millis();
  static unsigned long c_last_time = 0;
  unsigned long c_dt = c_now - c_last_time;
  static int goal_angle;
  if (c_last_time == 0) { //最初の微分が異様にデカくなるのを防ぐ
    c_last_time = c_now;
    return 0;
  }
  if (c_dt >= 300) {
    if (cx != 400 && abs(angleZ) < 900) {
      goal_angle = -angleZ + cx;
    }
    else {
      goal_angle = 0;
    }
    c_last_time =c_now;
  }
  return goal_angle;
}

void loop() {
  IMU();
  UART();
  Vector v = ballV();
  v = addV(v, lineV());
  int ball_get = analogRead(ball_get_sensor);
  moveVector(v, rotatePID(0, camera()));

  // moveVector(v, rotatePID(0, 50));
  // if (cx == 400) {
  //   moveVector(v, rotatePID(0, 0));
  // }
  // else {
  //   moveVector(v, rotatePID(1, cx));
  // }
  // moveVector(v, rotatePID(0, 0));
  
  Serial.print(angleZ);
  Serial.print(" : ");
  Serial.print(cx);
  Serial.print(" : ");
  Serial.println(camera());
}
