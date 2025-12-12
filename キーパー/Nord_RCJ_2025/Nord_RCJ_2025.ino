#include "functions.h"
#include "IMU.h"
#include "move.h"
#include "UART.h" // UARTはいろんな変数使うから最後にincludeする

void setup() {
  initMotor();
  initUART();
  initIMU();
}


const int NUM_SENSORS = 13;//
const float KdigDis[NUM_SENSORS] = {0, 0, 0, 0, 0, 0, 0.2, 0.6, 1, 1, 1, 1, 1}; // 0,1,2,3,...
//5以上が信頼性高い


const int FRONT_RANGE = 45;
const int SPEED = 200;
const int BALL_K = 40;
const int BALL_FRONT_K = 1.2;

int line_start = 400;

Vector ball_v;

unsigned long last_time = 0;
unsigned long now;
unsigned long dt;

int lastDis[4] = {0, 0, 0, 0};
float dWall[4] = {0, 0, 0, 0};
float wSpeed[4] = {0, 0, 0, 0};

void echo(){
  now = millis();
  dt = now - last_time;

  if (dt >= 30) { // 30ms以上経ったら計算
    for (int i = 0; i < 4; i++) {
      Serial.print(dis[i]);
      Serial.print(" : ");
      int diff = dis[i] - lastDis[i];
      dWall[i] = (float)diff * 1000.0 / dt;
      Serial.print(dWall[i]);
      Serial.print(" : ");
      int dir;
      if (i % 2 == 1) dir = ball_v.x; //ロボットで上下方向
      else dir = ball_v.y;  //ロボットで左右方向

      wSpeed[i] = 0;

      if (dis[i] == -1) {

      }
      else {
        if (dis[i] < 600) {
          wSpeed[i] = ((600 - dis[i]) / 600 * dir) + ((-dWall[i]) / 1000 * dir) * 1.4;

        }
      }

      lastDis[i] = dis[i];

      Serial.print(wSpeed[i]);
      Serial.print(" :::::: ");
    }
    last_time = now;
    Serial.println(" ");
  }
}

void loop() {
  dis[2] = -1;
  dis[3] = -1;
  // unsigned long time = millis();
  UART();
  IMU();
  //////////////////////////////////////////////////////
  // ball_angle = 90;
  //////////////////////////////////////////////////////
  if (ball_angle == 400) {
    ball_v = fromPolar(0, 0);
  }
  else {
    int lr = 1;
    if ((ball_angle - angleZ) < 0) lr = -1;
    ball_v = fromPolar((ball_angle - angleZ) + (ball_angle - angleZ) * 0.5 * KdigDis[ball_distance], SPEED);
  }
  
  // Vector wall_v = fromPolar(0, 0);
  
  // if (wall_dis == -1) {

  // }
  // else {
  //   if (wall_dis < 600) {

  //   unsigned long now = millis();
  //   unsigned long dt = now - last_time;

  //     if (dt >= 30) { // 30ms以上経ったら計算
  //       int diff = wall_dis - last_wall_dis;

  //       // mm/s に変換
  //       w_speed = (float)diff * 1000.0 / dt;

  //       last_time = now;
  //       last_wall_dis = wall_dis;
  //     }

  //     // Serial.print(wall_dis);
  //     // Serial.print(" : ");
  //     // Serial.println(w_speed);
  //     // * ((600 - wall_dis) / 600)
  //     wall_v = fromPolar(-90, ((600 - wall_dis) / 600 * ball_v.y )+ (-w_speed)/1000 * ball_v.y);
  //   }
  //   else {
  //     // wall_v = fromPolar(0, 0);
  //   }
  // }
  // echo();
  // Vector wall_U = fromPolar(180, wSpeed[1]);
  // Vector wall_R = fromPolar(-90, wSpeed[0]);
  // if (ball_v.x > 0) wall_v = add(wall_v, wall_U);
  // if (ball_v.y > 0) wall_v = add(wall_v, wall_R);
  
  Vector line_v = fromPolar(0, 0);

  if (line_angle != 400) {
    if (line_start == 400) {
      line_start = line_angle;
    }

    int x;
    if (abs(line_angle - line_start) > 140) {
      x = line_angle; // 最初の角度より大きく違ったら、半分越えたと判断本来の角度の逆方向を入れる
    }
    else {
      x = line_angle + 180; //　白線の反対側
    }
    line_v = fromPolar(x, ball_v.y + 200);
  }
  else {
    line_start = 400;
  }
  Vector v = ball_v;
  // Vector v = fromPolar(90, ball_v.y);  //横方向がball_v.y;
  // v = add(v, wall_v);
  // v = add(v, line_v);
  // v = lenV(v, 200);
  moveVector(v, rotatePID(0));

}