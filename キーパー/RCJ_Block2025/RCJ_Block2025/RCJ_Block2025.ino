
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

void sound_calSuccess(){
  digitalWrite(21, HIGH);
  delay(50);
  digitalWrite(21, LOW);
  delay(50);
  digitalWrite(21, HIGH);
  delay(30);
  digitalWrite(21, LOW);
}
void sound_beep(){
  digitalWrite(21, HIGH);
  delay(50);
  digitalWrite(21, LOW);
}
void setup() {
  pinMode(21, OUTPUT);//圧電スピーカー
  initUART();
  initIMU();
  initMotor();
  pinMode(ball_get_sensor, INPUT);
  sound_calSuccess();
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
float approach_to_ball = 220;
Vector ballV() {
  Vector v;
  if(ball_angle != 400){
    v = makeV(ball_angle,approach_to_ball);
  }
  else{
     v = makeV(0,0);
  }
  // //Serial.println(v.y);
  return v;
}

Vector lost_line  = makeV(180,220);
uint8_t line_tolerance = 15;//線をはみ出したと判断するline_distanceのしきい値
float approach_to_line = 0.90;//ライントレースの戻る力
float approach_to_line_out_of_line = 1.50;//ラインを見失った後の戻る力

  float read_line_angle_1;
  float read_line_angle_2;
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
  Vector v;
  if(line_dis>line_tolerance && abs(line_angle)<90){//白線が機体より前にある(ゴールエリア内にいる)
    v.x = v.x + line_dis * approach_to_line;
  }
  else if(line_dis>line_tolerance && abs(line_angle)>90){//白線が機体より後ろにある(前に出過ぎた)
    v.x = v.x - line_dis * approach_to_line;
  }
  if(line_angle != 400){
    lost_line = makeV(line_angle,line_dis*approach_to_line_out_of_line);
    read_line_angle_1 = line_angle;
    delay(2);
    UART();
    read_line_angle_2 = line_angle - read_line_angle_1;
    if(read_line_angle_2 > 0){
      sound_beep();
    }
    if(abs(line_angle)<100){
     line_tolerance = 20;//線をはみ出したと判断するline_distanceのしきい値
     approach_to_line = 1.30;//ライントレースの戻る力 
     approach_to_line_out_of_line = 2.00;
    }
    else{
     line_tolerance = 15;//線をはみ出したと判断するline_distanceのしきい値
     approach_to_line = 0.90;//ライントレースの戻る力
     approach_to_line_out_of_line = 1.50;
    }
  
  }
  return v;
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
/*
//line_angle+180度を求める
float oppositeAngle(float theta) {
  float a;
  // if(abs(theta)>90){
  //   a = theta;
  // }
  // else{
  //  a = theta + 180.0;
  //  if (a > 180.0) {
  //    a = -1 * (360 - a); 
  //   }   
  // }
     a = theta + 180.0;
   if (a > 180.0) {
     a = -1 * (360 - a); 
    }

  return a;
}


//line_angleは、機体正面に対する、
//機体中央から白線におろした垂線(最短距離)の直線の角度
float getCom(Vector v, float deg) {
  float rad = deg * PI / 180.0f;
  float how_approach = 0.5;
  return (v.x * cos(rad) + v.y * sin(rad))*how_approach;
}
*/

//モーターが動く最大値と最小値
const int PWM_min = 70;
const int PWM_max = 255;
float PercentToPWM(int percent){//百分率をPWMに変換(最大値、最小値は調整してください)
  return PWM_min + (PWM_max - PWM_min) * percent / 100;
}

float lineNormalization(float angle){//角度計算後の正規化
  float theta;
  if(angle > 180){
    angle = -1  * (360 - angle);
  }
  else if(angle < -180){
    angle = 360 - angle;
  }
  return angle;
}

float getRemoveAngle(){
  float theta;
  float line_left = lineNormalization(line_angle - 90);//白線に並行な線の角度(左)
  float line_right = lineNormalization(line_angle + 90);//白線に並行な線の角度(右)
  if(line_right < line_left){
      if(line_angle > 0){//+-180度をまたいだかどうか
        line_right = abs(180 + line_right) + 180; 
      }
      else{
        line_right = 360 - abs(line_right);  
      }
    }
    if(ball_angle >= line_left && ball_angle <= line_right){//白線から離れようとするベクトルの角度を出す(ロボットから見て白線の反対側にボールがあるとき)
      theta = lineNormalization(line_angle + 180);
    }
    else{//白線に近づこうとするベクトルの角度を出す(ボールが白線の向こう側にあるとき)
      theta = line_angle;
    }   
    
  }


float getRemovePower(int percent = 100){//percentにはボールへ向かう力を代入、デフォルトは100%(255)
  float theta;
    float line_left = lineNormalization(line_angle - 90);//白線に並行な線の角度(左)
  float line_right = lineNormalization(line_angle + 90);//白線に並行な線の角度(右)
  if(line_right > line_left){
    if(ball_angle >= line_left && ball_angle <= line_right){//白線から離れようとするベクトルの角度を出す(ロボットから見て白線の反対側にボールがあるとき)
      theta = 90 - abs(line_angle - ball_angle);
    }
    else{//白線に近づこうとするベクトルの角度を出す(ボールが白線の向こう側にあるとき)
      theta = 90 - abs(ball_angle - lineNormalization(line_angle + 180));
    }    
  }
  else{
    if(line_angle > 0){//+-180度をまたいだかどうか
      line_right = abs(180 + line_right) + 180; 
    }
    else{
      line_right = 360 - abs(line_right);  
    }
    if(ball_angle >= line_left && ball_angle <= line_right){//白線から離れようとするベクトルの角度を出す(ロボットから見て白線の反対側にボールがあるとき)
    //  line_right = lineNormalization(line_angle_rev + 90);
    //  line_left = lineNormalization(line_angle_rev - 90);
      theta = 90 - abs(ball_angle - lineNormalization(line_angle + 180));
    }
    else{//白線に近づこうとするベクトルの角度を出す(ボールが白線の向こう側にあるとき)
        theta = 90 - abs(line_angle - ball_angle);
    }        
  }
  return 255 * percent * ball_dis * sin(theta);
}


Vector v;

float remove_angle;
void loop() {
 Vector remove;  
   IMU();
   UART();
  if(line_angle == 400){
    v = lost_line;      
    remove = makeV(0,0);
   }
  else
  {
    remove = makeV(getRemoveAngle(),getRemovePower());
    v  = ballV();
    v  = addV(v, lineV());
    v  = addV(v , remove);
  }
  moveVector(v, rotatePID(0, 0));

   Serial.print("removeAngle:");Serial.print(remove_angle);
   //Serial.print(",getCom:");Serial.print(getCom(ballV(),remove_angle));
   Serial.print(",Ball_angle,dis=(:");Serial.print(ball_angle);
   Serial.print(",");Serial.print(ball_dis);
   Serial.print("),");
   Serial.print(",Line_angle,dis=(:");Serial.print(line_angle);
   Serial.print(",");Serial.print(line_dis);
   Serial.print("), v:");
   Serial.print("(");
   Serial.print(v.x);Serial.print(",");Serial.print(v.y);
   Serial.print(")");
   Serial.print(",dxLine:"); Serial.println(read_line_angle_2);
  //Serial.println(v);
}
