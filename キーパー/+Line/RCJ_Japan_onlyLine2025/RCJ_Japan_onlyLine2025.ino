#include "UART.h"
#include "IMU.h"
#include "functions.h"
const int beep = 21;
int count_outofline = 0;
int debugState = 0;
bool is_last_T = false;
int last_line_angle_tate;
//モーターが動く最大値と最小値
float PercentToPWM(float percent = 100,float PWM_min=70,float PWM_max=255){//百分率をPWMに変換(最大値、最小値は調整してください)
    return PWM_min + (PWM_max - PWM_min) * percent / 100;
}

//以下重要な係数/////////////////////////////////////
float lost_angle = 180;
Vector lost_line  = makeV(lost_angle,PercentToPWM(30));
uint8_t line_tolerance = 20;//線をはみ出したと判断するline_distanceのしきい値
float approach_to_line = 0.85;//ライントレースの戻る力
float approach_to_line_out_of_line = 90;//ラインを見失った後の戻る力(最後のline_disの何倍か)

float approach_to_ball = 80;//ボールを追う力(%)


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
//打ち消しここから//////////////////////////////////////////////////////
float angleTo360(float theta){
  if(theta < 0){
    theta = 360 + theta;
  }
  else{
    theta;
  }
  return theta;
}
float angleTo180(float theta){
  if(theta > 180){
    theta = theta - 360;
  }
  else{
    theta;
  }
  return theta;
}
float lineNormalization360(float theta){
  if(theta > 360){
    theta = theta - 360;
  }
  else if(theta < 0){
    theta = theta + 360;
  }
  else {
    theta;
  }
  return theta;
}
float lineNormalization180(float angle){//角度計算後の正規化
  float theta;
  if(angle > 180){
    angle = -1  * (360 - angle);
  }
  else if(angle < -180){
    angle = 360 - abs(angle);
  }
  return angle;
}
float ball_angle_from_line360;
Vector removeV(float &remove_angle, float &remove_power,float line_angle, float ball_angle, Vector ballV){
  float line_angle360 = angleTo360(line_angle);
  float ball_angle360 = angleTo360(ball_angle);

  if(line_angle == 400 || ball_angle == 400){
    remove_power = 0;
  }
  else{
    float line_left = lineNormalization360(line_angle360 - 90);
    float line_right = lineNormalization360(line_angle360 + 90);
    if(line_right < line_left){//正面の0度をまたいだとき
      if(ball_angle360 >= line_left || ball_angle360 <= line_right){//ボールと自分の間にラインあり
          remove_angle = lineNormalization360(line_angle360 + 180);
          ball_angle_from_line360 = ball_angle360 - line_angle360; 
      }
      else{
        remove_angle = line_angle360;
        ball_angle_from_line360 = ball_angle360 - lineNormalization360(line_angle360 + 180);
      }
      if(abs(ball_angle_from_line360) > 180){
        ball_angle_from_line360 = (360 - ball_angle_from_line360) * ball_angle_from_line360/abs(ball_angle_from_line360);
      }
    }
    else{//またいでいないとき
      if(ball_angle360 > line_left && ball_angle360 < line_right){//ボールと自分の間にラインあり
          remove_angle = lineNormalization360(line_angle360 + 180);
          ball_angle_from_line360 = ball_angle360 - line_angle360; 
      }
      else{//ボールが前、線が後ろ
        remove_angle = line_angle360;
        ball_angle_from_line360 = lineNormalization360(ball_angle360 - lineNormalization360(line_angle360 + 180));
      }
      if(abs(ball_angle_from_line360) > 180){
        ball_angle_from_line360 = (360 - ball_angle_from_line360) * -1;
      }
    }
    remove_power = sqrt(ballV.x * ballV.x + ballV.y * ballV.y) *1.00 * cos(ball_angle_from_line360 * PI/180);
  }
  return makeV(remove_angle,abs(remove_power));
}
//打ち消しここまで//////////////////////////////////////////////////////

// int camera() {
//   unsigned long c_now = millis();
//   static unsigned long c_last_time = 0;
//   unsigned long c_dt = c_now - c_last_time;
//   static int goal_angle;
//   if (c_last_time == 0) { //最初の微分が異様にデカくなるのを防ぐ
//     c_last_time = c_now;
//     return 0;
//   }
//   if (c_dt >= 300) {
//     if (c_x != 400 && abs(angleZ) < 900) {
//       goal_angle = -angleZ + c_x;
//     }
//     else {
//       goal_angle = 0;
//     }
//     c_last_time =c_now;
//   }
//   return goal_angle;
// }

// Vector cameraV(Vector v , uint16_t curve_start, uint16_t curve_end, uint16_t curve_out){
//   float    cam_remove_Percent;
//   uint16_t cam_remove_angle;
//   if(c_x != 400){
//     if(abs(c_x) >= curve_start){
//           float a =
//               (100.0f * (curve_out - 2*curve_end + curve_start)) /
//               ((curve_end - curve_out) *
//                (curve_start - curve_out) *
//                (curve_start - curve_end));

//           float b =
//               (curve_out*curve_out
//                - 2*curve_end*curve_end
//                + curve_start*curve_start)
//               / (2.0f * (curve_out - 2*curve_end + curve_start));

//           cam_remove_Percent = a * (abs(c_x) - b) * (abs(c_x) - b);
//           if(cam_remove_Percent<0){cam_remove_Percent = 0;}

//      if(c_x < 0){cam_remove_angle = -90;}else{cam_remove_angle = 90;}
     
//     }
//     else{
//       cam_remove_Percent = 0;
//     }
//   }
//   v = makeV(cam_remove_angle,PercentToPWM(cam_remove_Percent,0,255));
//   return v;
// }

// Vector cameraV(Vector v , uint16_t curve_start, uint16_t curve_end, uint16_t curve_out){
//   float    cam_remove_Percent;
//   uint16_t cam_remove_angle;
//   if(c_x != 400){
//     if(abs(c_x) >= curve_start){
//           cam_remove_Percent = abs(c_x) * abs(c_x) /800;
//           if(cam_remove_Percent<0){cam_remove_Percent = 0;}

//      if(c_x < 0){cam_remove_angle = -90;}else{cam_remove_angle = 90;}
     
//     }
//     else{
//       cam_remove_Percent = 0;
//     }
//   }
//   v = makeV(cam_remove_angle,PercentToPWM(cam_remove_Percent,0,255));
//   return v;
// }

float camera_linetracing_brake(int c_x,Vector v , uint16_t curve_start, uint16_t curve_end, uint16_t curve_out){
  float power = 0;//百分率
  if(c_x == 400){
    power == 0.8;
  }
  else{
    if(abs(c_x)>curve_start && abs(c_x)<curve_end){
        power =  (abs(c_x) - curve_start) / (float)(curve_end - curve_start);
        if(c_x < curve_start){
          power = power * -1;
        }
      }
      else if(abs(c_x)>=curve_end){
        power = 1;
        if(c_x < curve_start){
          power = -1;
       }
  }
  }
  return power * PercentToPWM(80);
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
    v = makeV(ball_angle,PercentToPWM(approach_to_ball));
  }
  else{
     v = makeV(0,0);
  }
  return v;
}


float lineDf_dis;
float lineDf_angle;
Derivative lineD_dis;
Derivative lineD_angle;
bool isOnCurve = false;
float last_angle;
Vector lineV(){
  Vector v = makeV(0,0);

  //400, 500の処理はloopに書く
    isOnCurve = false;
    lineDf_dis = getD(lineD_dis,line_dis,30);//第3引数はms
    lineDf_angle = 0;
    if(line_angle != 0 && line_angle !=180){
      lineDf_angle=getD(lineD_angle,line_angle,30);
    }
       
    //白線に戻る動き
    float Power_lostline  = abs(lineDf_dis)*0.0005 * 100;
    if(Power_lostline>100){Power_lostline=100;}
    lost_line = makeV(line_angle,PercentToPWM(Power_lostline,140,180));
    last_angle = line_angle;
    if(lineDf_angle > 300 && v.x < 0){//曲がり角後退対策
      isOnCurve = true;
    }

    //白線の中央にいる動き
    float Power_linetrace;
    if(line_dis > 60){
      Power_linetrace = line_dis*line_dis * 0.0001 * 70 + abs(lineDf_dis) * 0.001 * 30;
    }
    else{
       Power_linetrace = abs(lineDf_dis) * 0.001 * 100;
    }
    if(Power_linetrace>100){Power_linetrace=100;}//100%以上の異常値対策
    v = makeV(line_angle,PercentToPWM(Power_linetrace,60,140));
  
  return v;
}

Vector EchoV(uint16_t dis_front, uint16_t dis_right, uint16_t dis_back, uint16_t dis_left){//ライントレース用(使わないかも)
  Vector v;
  return v;
}
Vector lostEchoV(uint16_t dis_front, uint16_t dis_right, uint16_t dis_back, uint16_t dis_left){//line_angle = 400のときのみ
  Vector v;
  return v;
}

Vector lostGoalV(int c_x){
  Vector v;
  float move_angle;
  float move_power;
  if(abs(c_x) < 30){
    move_angle = 0;
  }
  else{
    move_angle = c_x * 1.5;
  }
  
  if(c_x == 400){
    move_power = 0;
  }
  else{
    move_power = PercentToPWM(30);
  }
  v = makeV(move_angle,move_power);
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

void motorDebug() {
  static int i = 0;
  static unsigned long lastTime = 0;
  const unsigned long interval = 300;

  unsigned long now = millis();

  if (now - lastTime >= interval) {
    lastTime = now;

    Vector v;

    int value = i * 8.5 * ((i % 2 == 1) ? 1 : -1);

    v = makeV(0, value);
    moveVector(v, rotatePID(0, 0));
    Serial.println(value);

    i++;

    // ★ 一周したら最初へ戻す
    if (i >= 30) {
      i = 0;
    }
  }
}


// void loop(){
//   // motorDebug();
//   moveVector(lostCameraV(c_x), rotatePID(0, 0));
//  }
float remove_angle;
float remove_power;//打ち消し
uint16_t lost_count = 0;
void loop() {//beep
 Vector remove;
   IMU();
   UART();
  if(line_angle == 400){//ライン未検出時
    debugState = 6;
    if(abs(lost_angle) < 100 && abs(lost_angle) > 30 || lost_count >= 1000){//lost_countしきい値要調整 //角から出たときor復帰時
      v = lostGoalV(c_x);
    }
    else{
      lost_count++;
      v = lost_line;  
    }
    remove = makeV(0,0);
   }
  else
  {                    //ライン検出時
    lost_count = 0;
    debugState = 1;
    v  = ballV();
    v  = addV(v, lineV());   
    v  = addV(v , removeV(remove_angle,remove_power,line_angle,ball_angle,ballV()));
    v.y = v.y + camera_linetracing_brake(c_x,v,90,130,130);
  //  v  = cameraV(v,50,110,145); 
    if(line_angle==500){
      v.y = 0;
      if(v.x<0)
        v.x = -1.5 * v.x;
    }
  //  v = echoV(v,line_angle,cx,dis_back,dis_right,dis_left); 
  if(abs(line_angle) < 120 && v.x <0 ){
    v.x = 0;
  }

    //v.x = v.x * 0.8;
  }
  //if(line_angle < 400 && isOnCurve){v.x = v.x*0.6;}
  moveVector(v, rotatePID(0, 0));

   
  //デバッグ
  // printState(debugState);
  //  Serial.print(",lineD_dis:");Serial.print(lineDf_dis);
   Serial.print(", cx:");Serial.print(c_x);
   Serial.print(", camBrake:");Serial.print(camera_linetracing_brake(c_x,v,50,100,150));
   Serial.print("cameraV:(");Serial.print(lostCameraV().x);Serial.print(",");Serial.print(lostCameraV().y);
  //  Serial.print(",");Serial.print(c_y);
  //  Serial.print(",");Serial.print(c_s);Serial.print(",");
  //  Serial.print("rem:");Serial.print(cameraV(v,50,110,145).y);
  //  Serial.print(" ,F:");Serial.print(dis_front);Serial.print("mm");
  //  Serial.print(",B:");Serial.print(dis_back);Serial.print("mm");
  //  Serial.print(",R:");Serial.print(dis_right);Serial.print("mm");
  //  Serial.print(",L:");Serial.print(dis_left);Serial.print("mm ");
  //  Serial.print(",Ball_angle,dis=(:");Serial.print(ball_angle);
  //  Serial.print(",");Serial.print(ball_dis);
  // //  Serial.print("),");
  //   Serial.print(", Line_angle,dis=(:");Serial.print(line_angle);
  //   Serial.print(",");Serial.print(line_dis);
  //   Serial.print(",  Lball:");
  //   Serial.print(ball_angle_from_line360);
  //   Serial.print(" ,Lsin:");Serial.print(sqrt(ballV().x * ballV().x + ballV().y * ballV().y) *1.00 * cos(abs(ball_angle_from_line360)*PI/180));
  //   Serial.print(" ,angle:");Serial.print(remove_angle);Serial.print(" ,dis:");
  //   Serial.print(remove_power);
  // //宣言したDerivative型変数、微分対象、時間
  // // dis_back_d = getD(echoD_back, dis_back, 30);
  // //  Serial.print(", backD:");Serial.print(dis_back_d);
  //  Serial.print(" ,last:");
  //  Serial.print("(");
  //  Serial.print(lost_line.x);Serial.print(",");Serial.print(lost_line.y);Serial.print(",");Serial.print(lost_angle);
  //  Serial.print(")");
  //  Serial.print(" ,v:");
  //  Serial.print("(");
  Serial.print(",  ");
   Serial.print(v.x);Serial.print(",");Serial.print(v.y);
  //  Serial.print(")");


  //ライントレースデバッグ用
  //    Serial.print(line_dis * line_dis * 0.0001 * 70);Serial.print(" + ");
  //  Serial.print(abs(lineDf_dis)*0.001 * 30);Serial.print(" = ");
  //  Serial.print(line_dis * 0.01 * 50 + abs(lineDf_dis) * 0.001 * 50);
  //  Serial.print(" , lost:");Serial.print(abs(lineDf_dis)*0.0005 * 100);
  //  Serial.print("angleD:");Serial.print(lineDf_angle);
  //  Serial.print("lastAngle:");Serial.print(last_angle);
  //  Serial.print(",Line_angle,dis=(");Serial.print(line_angle);Serial.print(":");Serial.print(line_dis);
  //  Serial.print(",lineD_dis:");Serial.print(lineDf_dis);
  //  Serial.print(" ,v:");
  //  Serial.print("(");
  //  Serial.print(v.x);Serial.print(",");Serial.print(v.y);
  //  Serial.print(")");
    Serial.println();

}
