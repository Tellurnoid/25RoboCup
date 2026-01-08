#include "UART.h"
#include "IMU.h"
#include "functions.h"


//モーターが動く最大値と最小値
const int PWM_min = 70;
const int PWM_max = 255;
float PercentToPWM(int percent){//百分率をPWMに変換(最大値、最小値は調整してください)
  return PWM_min + (PWM_max - PWM_min) * percent / 100;
}

//以下重要な係数/////////////////////////////

Vector lost_line  = makeV(180,PercentToPWM(80));
uint8_t line_tolerance = 16;//線をはみ出したと判断するline_distanceのしきい値
float approach_to_line = 1.00;//ライントレースの戻る力
float approach_to_line_out_of_line = 1.00;//ラインを見失った後の戻る力
 float remove_power = 0.90;//打ち消し
float approach_to_ball = PercentToPWM(100);//ボールを追う力
////////////////////////////////////////////////////

float getRad(float deg) {
  return deg * PI / 180.0;
}

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
  //sound_calSuccess();
}

//const int NUM_SENSORS = 13;

Derivative ballD;
Vector ballV() {
  Vector v;
  if(ball_angle != 400){
    v = makeV(ball_angle,approach_to_ball);
  }
  else{
     v = makeV(0,0);
  }
  return v;
}



  // float read_line_angle_1;
  // float read_line_angle_2;
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

    // if(abs(line_angle)<100){
    //  line_tolerance = 20;//線をはみ出したと判断するline_distanceのしきい値
    //  approach_to_line = 1.30;//ライントレースの戻る力 
    //  approach_to_line_out_of_line = 2.00;
    // }
    // else{
    //  line_tolerance = 15;//線をはみ出したと判断するline_distanceのしきい値
    //  approach_to_line = 0.90;//ライントレースの戻る力
    //  approach_to_line_out_of_line = 1.50;
    // }
  
  }
  return v;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//超音波
Derivative echoD_back;
 float dis_back_d = getD(echoD_back, dis_back, 30);//宣言したDerivative型変数、微分対象、時間




const int echo_wall = 180;
const int echo_wall_another_side = 120;
const float echo_wall_rep = 250;
Vector echoV(){
  Vector v = makeV(0,0);
  
  //横の壁に対する反発
  if(dis_left < echo_wall && dis_right > echo_wall_another_side){//左壁
    //sound_beep();
    v.y =  255 * (echo_wall_rep - dis_left) / echo_wall;
  }
  else if(dis_right < echo_wall && dis_left > echo_wall_another_side){//右壁
  //sound_beep();
    v.y =  -255 * (echo_wall_rep - dis_right) / echo_wall;
  }
  if(dis_front < echo_wall && dis_back > echo_wall_another_side){//前壁
 // sound_beep();
    v.x =  -255 * (echo_wall_rep - dis_front) / echo_wall;
  }
  else if(dis_back < echo_wall && dis_front > echo_wall_another_side){//後壁
  //  sound_beep();
    v.x =  255 * (echo_wall_rep - dis_back) / echo_wall;
  }
  return v;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

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

//const float pi = 3.14;
//float border_angle[];
//float go_x,go_y;
//float go_angle;
//int go_flag;//進むベクトルの成分
//進む角度
//左右どちらに進むかのフラグ
//float l_x,l_y;
//float line_coeffident = 1.0;
//float theta = atan2(l_y,l_x);//白線に対するベクトルの成分


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

float getRemoveAngle(float line_angle,float ball_angle){//打ち消し用。引数なし
  float theta;
  float line_left = lineNormalization(line_angle - 90);//白線に並行な線の角度(左)
  float line_right = lineNormalization(line_angle + 90);//白線に並行な線の角度(右)
  if(line_right < line_left){
      if(line_angle > 0){//+-180度をまたいだかどうか
        line_right = abs(180 + line_right) + 180; 
      }
      else{
        line_right = 360 + line_right;  
      }
    }
  if(ball_angle >= line_left && ball_angle <= line_right){//白線から離れようとするベクトルの角度を出す(ロボットから見て白線の反対側にボールがあるとき)
      theta = lineNormalization(line_angle + 180);
    }
    else{//白線に近づこうとするベクトルの角度を出す(ボールが白線の向こう側にあるとき)
      theta = line_angle;
    }   
  return theta;  
  }



float theta;  
float getRemovePower(float percent,float line_angle,float ball_angle){//打ち消し用。percentにはボールへ向かう力を代入
  float line_left = lineNormalization(line_angle - 70);//白線に並行な線の角度(左)
  float line_right = lineNormalization(line_angle + 65);//白線に並行な線の角度(右)
  if(ball_angle == 400){
    return 0;  
  }
  else{
    if(line_right > line_left){
      if(ball_angle >= line_left && ball_angle <= line_right){//白線から離れようとするベクトルの角度を出す(ロボットから見て白線の反対側にボールがあるとき)
        theta = 90 - abs(line_angle - ball_angle);
      }
      else{//白線に近づこうとするベクトルの角度を出す(ボールが白線の向こう側にあるとき)
        theta = 90 - abs(ball_angle - lineNormalization(line_angle + 160));
      }    
    }
    else
    {
      if(line_angle > 0){//+-180度をまたいだかどうか
        line_right = abs(180 + line_right) + 160; 
      }
      else
      {
        line_right = 360 - abs(line_right);  
      }
      if(ball_angle >= line_left && ball_angle <= line_right){//白線から離れようとするベクトルの角度を出す(ロボットから見て白線の反対側にボールがあるとき)
        theta = 90 - abs(ball_angle - lineNormalization(line_angle + 160));
      }
      else{//白線に近づこうとするベクトルの角度を出す(ボールが白線の向こう側にあるとき)
          theta = 90 - abs(line_angle - ball_angle);
      }        
    }
    return abs(percent * sin(getRad(theta)));//÷100
  }
}


Vector v;

void loop() {//beep
 Vector remove;
   IMU();
   UART();

  if(line_angle == 400){
    v = lost_line;      
    remove = makeV(0,0);
   }
  else
  {
    v  = ballV();Serial.print("echo().x = ");Serial.print(echoV().x);
    v  = addV(v, lineV());   
    remove = makeV(getRemoveAngle(line_angle,ball_angle),remove_power*getRemovePower(approach_to_ball,line_angle,ball_angle));
    v  = addV(v , remove);
    //v  = addV(v, echoV()); 
  }

  // Vector wall_v = makeV(0,0);
  // echo();
  // Vector wall_U = makeV(180, wSpeed[1]);
  // Vector wall_R = makeV(-90, wSpeed[0]);
  // if (ballV().x > 0) wall_v = addV(wall_v, wall_U);
  // if (ballV().y > 0) wall_v = addV(wall_v, wall_R);
  // v = wall_v;
  // //v = addV(v, ballV());

  moveVector(v, rotatePID(0, 0));

    // Serial.print("echo()= ");Serial.print(v.x);Serial.print(",");Serial.print(v.y);
    // Serial.print(",front:");Serial.print(dis_front);Serial.print("mm");
    // Serial.print("back:");Serial.print(dis_back);Serial.print("mm");
    // Serial.print("right:");Serial.print(dis_right);Serial.print("mm");
    // Serial.print("left:");Serial.print(dis_left);Serial.print("mm");
   Serial.print("removeAngle,power:");Serial.print(getRemoveAngle(line_angle,ball_angle));Serial.print(",");Serial.print(remove_power*getRemovePower(approach_to_ball,line_angle,ball_angle));
  //  //Serial.print(",getCom:");Serial.print(getCom(ballV(),remove_angle));
   Serial.print(",Ball_angle,dis=(:");Serial.print(ball_angle);
   Serial.print(",");Serial.print(ball_dis);
   Serial.print("),");
    Serial.print(",Line_angle,dis=(:");Serial.print(line_angle);
    Serial.print(",");Serial.print(line_dis);
    Serial.print(", backD:");Serial.print(dis_back_d);
  //   Serial.print("app_to_ball");Serial.print(approach_to_ball);
  //   Serial.print(",ball_dis:");Serial.print(ball_dis/6);
  //   Serial.print(",sin:");Serial.print(abs(sin(theta)));
  //  Serial.print("), v:");
  //  Serial.print("(");
  //  Serial.print(v.x);Serial.print(",");Serial.print(v.y);
  //  Serial.println(")");
Serial.println();
}
