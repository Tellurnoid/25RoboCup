
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


float getCom(Vector v, float deg) {
  float rad = deg * PI / 180.0f;
  return v.x * cos(rad) + v.y * sin(rad);
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
// const float KdigDis[NUM_SENSORS] = {0, 0, 0, 0, 0, 15, 30, 30, 45, 45, 60, 90, 90}; // 0,1,2,3,...

// const int FRONT_RANGE = 40;
// const int Kp_ball = 1;
// const int Kd_ball = 2.7;
// const int D_MIN = 300;

// const int SPEED = 250;  // 普通のボールチェイス時
// const int x_max_speed = 250;  // FRONT_RANG内の時
// const int y_max_speed = 250;  //

Derivative ballD;
float approach_to_ball = 180;
Vector ballV() {
  Vector v;
  
  v = makeV(ball_angle,approach_to_ball);
  //Serial.println(v.y);
  return v;
}


Vector lost_line  = makeV(180,180);
uint8_t line_tolerance = 15;//線をはみ出したと判断するline_distanceのしきい値
float approach_to_line = 0.90;//ライントレースの戻る力
Vector lineV() {
  /*
  ○角度
    前：0度
    後：+-180度
    右：90度
    左：-90度
  line_dis>0　&& abs(line_angle)<90　のとき、白線は機体より前に
  line_dis>0　&& abs(line_angle)>90　のとき、白線は機体より後ろに
  abs(line_angle)=90　⇔　line_dis=0  ⇒まっすぐライントレース
  */
  Vector lineV_for_cal = makeV(0,0);

  if(line_dis>line_tolerance && abs(line_angle)<90){//白線が機体より前にある(ゴールエリア内にいる)
    lineV_for_cal.x = lineV_for_cal.x + line_dis * approach_to_line;
  }
  else if(line_dis>line_tolerance && abs(line_angle)>90){//白線が機体より後ろにある(前に出過ぎた)
    lineV_for_cal.x = lineV_for_cal.x - line_dis * approach_to_line;
  }
  lost_line = makeV(line_angle,line_dis * 2);
  return lineV_for_cal;
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

const float pi = 3.14;
//float border_angle[];
float go_x,go_y;
float go_angle;
int go_flag;//進むベクトルの成分
//進む角度
//左右どちらに進むかのフラグ
float l_x,l_y;
float line_coeffident = 1.0;
float theta = atan2(l_y,l_x);//白線に対するベクトルの成分
//白線に留まる成分の強さ
//白線に対するベクトルとロボット正面方向の偏角float n_x,n_y;//白線と並行移動するベクトルの成


//line_angleは、機体正面に対する、
//機体中央から白線におろした垂線(最短距離)の直線の角度
Vector v;

void loop() {
  IMU();
  UART();
  if(line_angle == 400){
    v = lost_line;      
  }
  else
  {
    v = ballV();
      // v = lineV();
    v = addV(v, lineV());
  }
  //  v = addV(v, lineV());
   Vector remove = makeV(180,approach_to_ball);
   v  = addV(v , remove);
  int ball_get = analogRead(ball_get_sensor);
  moveVector(v, rotatePID(0, 0));
  // Serial.print("angle:");Serial.print(line_angle);
  // Serial.print(", dis:");Serial.print(line_dis);
   Serial.print(", v:");
   Serial.print("(");
   Serial.print(v.x);Serial.print(",");Serial.print(v.y);
   Serial.println(")");
  //Serial.println(v);
}
