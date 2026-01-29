#include "UART.h"
#include "IMU.h"
#include "functions.h"
const int beep = 21;
int count_outofline = 0;
int debugState = 0;
//モーターが動く最大値と最小値
const int PWM_min = 70;
const int PWM_max = 255;
float PercentToPWM(int percent){//百分率をPWMに変換(最大値、最小値は調整してください)
    return PWM_min + (PWM_max - PWM_min) * percent / 100;
}

//以下重要な係数/////////////////////////////////////

Vector lost_line  = makeV(180,PercentToPWM(20));
uint8_t line_tolerance = 15;//線をはみ出したと判断するline_distanceのしきい値
float approach_to_line = 1.10;//ライントレースの戻る力
float approach_to_line_out_of_line = 1.00;//ラインを見失った後の戻る力(最後のline_disの何倍か)
 float remove_power = 1.00;//打ち消し
float approach_to_ball = PercentToPWM(100);//ボールを追う力

int switch_to_camera = 70;//ラインに戻る動きからゴールへ向かう動きへ切り替えるカウントのしきい値(小さいほどすぐカメラに切り替わる)

//超音波
//int churitsuten = 580; //(超音波)自陣ゴールから最も近い中立点から後ろの壁の距離
const int echo_wall_back = 120;
const int echo_wall_right_and_left = 300;
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
float getD(Derivative &d, float value, int time) {//微分
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
void sound_calSuccess(){
  digitalWrite(beep, HIGH);
  delay(50);
  digitalWrite(beep, LOW);
  delay(50);
  digitalWrite(beep, HIGH);
  delay(30);
  digitalWrite(beep, LOW);
}
void sound_beep(){
  digitalWrite(beep, HIGH);
  delay(50);
  digitalWrite(beep, LOW);
}


//打ち消し//////////////////////////////////////////////////////////////
float lineNormalization(float angle){//角度計算後の正規化
  float theta;
  if(angle > 180){
    angle = -1  * (360 - angle);
  }
  else if(angle < -180){
    angle = 360 - abs(angle);
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
//打ち消しここまで//////////////////////////////////////////////////////

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

Vector notToOwnGoal(Vector v){//オウンゴール対策
  if(abs(ball_angle) > 160 && line_angle !=400){//ボールが背後にあれば7割の速度に
    v.x = v.x * 0.7;
    v.y = v.y * 0.7;
  }
  return v;
}

Vector ballV() {
  Vector v = makeV(0,0);
  if(ball_angle != 400){
    v = makeV(ball_angle,approach_to_ball);
  }
  else{
     v = makeV(0,0);
  }
  return v;
}

Vector lineV() {
  Vector v = makeV(0,0);
  if(line_dis>line_tolerance && abs(line_angle)<90){//白線が機体より前にある(ゴールエリア内にいる)
    v.x = v.x + line_dis * approach_to_line;
  }
  else if(line_dis>line_tolerance && abs(line_angle)>90){//白線が機体より後ろにある(前に出過ぎた)
    v.x = v.x - line_dis * approach_to_line;
  }

  if(line_angle != 400){
    lost_line = makeV(line_angle,line_dis*approach_to_line_out_of_line);
  }
  else{
    v = lost_line;
  }
  return v;
}


// Vector echoV(Vector v,int16_t line_angle,int cx,int dis_back,int dis_right,int dis_left){
//        if(dis_left < echo_wall_right_and_left && (cx < -130 || abs(90-abs(line_angle)) < 45)){//ゴールが視野の端かつ左壁にぶつかりそう
//          v.y = PercentToPWM(100);
//        }
//        else if(dis_right < echo_wall_right_and_left && (cx > 130 || abs(90-abs(line_angle)) < 45)){//ゴールが視野の端かつ右壁にぶつかりそう
//          v.y = -1 * PercentToPWM(100);
//        }
//        if(dis_back < 120){//後ろの壁にぶつかりそう
//          v.x = PercentToPWM(100);
//        }
//      return v;
// }
Vector echoV(Vector v,int16_t line_angle,int cx,int dis_back,int dis_right,int dis_left){
       if(dis_left < echo_wall_right_and_left && abs(90-abs(line_angle)) < 45){//ゴールが視野の端かつ左壁にぶつかりそう
        v.y = PercentToPWM(100);
       }
       else if(dis_right < echo_wall_right_and_left && abs(90-abs(line_angle)) < 45){//ゴールが視野の端かつ右壁にぶつかりそう
         v.y = -1 * PercentToPWM(100);
       }
       if(dis_back < 170){//後ろの壁にぶつかりそう
         v.x = PercentToPWM(100);
       }
     return v;
}
Vector lostGoalV(Vector v,int16_t line_angle,int cx,int dis_back,int dis_right,int dis_left){
   v = makeV(0,0);
       if(dis_left < echo_wall_right_and_left){//ゴールが視野の端かつ左壁にぶつかりそう
        v.y = PercentToPWM(100);
       }
       else if(dis_right < echo_wall_right_and_left){//ゴールが視野の端かつ右壁にぶつかりそう
         v.y = -1 * PercentToPWM(100);
       }
       if(dis_back < 160){//後ろの壁にぶつかりそう
         v.x = PercentToPWM(100);
       }
      
      else if(dis_left < dis_right){
        v = makeV(90,PercentToPWM(30));
      }
      else if(dis_left > dis_right){
      v = makeV(-90,PercentToPWM(30));
  }
      return v;
    }
  


void setup() {
  pinMode(beep, OUTPUT);//圧電スピーカー
  initUART();
  initIMU();
  initMotor();
}

Vector v;

void printState(int val){
  Serial.print("state:");
  switch(val){
    case 0:
     Serial.print("out of running      ");
     break;
    case 1:
     Serial.print("line tracing        ");
     break;
    case 2:
     Serial.print("keeper dash         ");
     break;
    case 3:
     Serial.print("back to goal noCam  ");
     break;
    case 4:
     Serial.print("back to goal withCam");//20文字
     break;
    case 5:
     Serial.print("calibrating..       ");
     break;
    case 6:
     Serial.print("lost line           ");
     break;
    default:
      Serial.print("unknown state       ");
      break;
  }
}

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
    v  = ballV();
    v  = addV(v, lineV());   
    remove = makeV(getRemoveAngle(line_angle,ball_angle),remove_power*getRemovePower(approach_to_ball,line_angle,ball_angle));
    v  = addV(v , remove);
  //  v = echoV(v,line_angle,cx,dis_back,dis_right,dis_left); 
    v.x = v.x * 0.8;
  }
  // if(line_angle == 400){
  //   if(dis_back > 500){
  //        v.x = -1 * PercentToPWM(70);
  //        v.y = cx * 1.2;
  //        debugState = 4;
  //        v = lostGoalV(v,line_angle,cx,dis_back,dis_right,dis_left);
  //   }
  //   else{
  //   v = lost_line;
  //   v.x = v.x * 1.5;      
  //   remove = makeV(0,0);
  //   }
  //  }
  // else
  // {
  //   debugState = 1;
  //   v  = ballV();
  //   v  = addV(v, lineV());   
  //   remove = makeV(getRemoveAngle(line_angle,ball_angle),remove_power*getRemovePower(approach_to_ball,line_angle,ball_angle));
  //   v  = addV(v , remove);
  //    v = echoV(v,line_angle,cx,dis_back,dis_right,dis_left); 
  // }
  moveVector(v, rotatePID(0, 0));

   
  //デバッグ
  printState(debugState);
   Serial.print(",count:");Serial.print(count_outofline);
   Serial.print(", cx:");Serial.print(cx);
   Serial.print(" ,F:");Serial.print(dis_front);Serial.print("mm");
   Serial.print(",B:");Serial.print(dis_back);Serial.print("mm");
   Serial.print(",R:");Serial.print(dis_right);Serial.print("mm");
   Serial.print(",L:");Serial.print(dis_left);Serial.print("mm ");
   Serial.print(",Ball_angle,dis=(:");Serial.print(ball_angle);
   Serial.print(",");Serial.print(ball_dis);
   Serial.print("),");
   Serial.print(",Line_angle,dis=(:");Serial.print(line_angle);
   Serial.print(",");Serial.print(line_dis);
   Serial.print("remove_Angle,power:");Serial.print(getRemoveAngle(line_angle,ball_angle));Serial.print(",");Serial.print(remove_power*getRemovePower(approach_to_ball,line_angle,ball_angle));
  //宣言したDerivative型変数、微分対象、時間
  // dis_back_d = getD(echoD_back, dis_back, 30);
  //  Serial.print(", backD:");Serial.print(dis_back_d);
   Serial.print(" ,last:");
   Serial.print("(");
   Serial.print(lost_line.x);Serial.print(",");Serial.print(lost_line.y);
   Serial.print(")");
   Serial.print(" ,v:");
   Serial.print("(");
   Serial.print(v.x);Serial.print(",");Serial.print(v.y);
   Serial.print(")");
   Serial.println();
}
