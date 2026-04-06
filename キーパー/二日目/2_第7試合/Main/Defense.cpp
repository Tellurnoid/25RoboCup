#include <Arduino.h>
#include "Instance.h"
#include "Defense.h"
#include <Adafruit_NeoPixel.h>

//デバッグ////////////////////////////////////////////////////////////////////////////////////
void Defense::debugSerial(){
  Serial.print("is_on_yoko:");
  Serial.print(is_on_yoko);
  Serial.print(", State:");
  Serial.print(state);
  Serial.print(" (");
  Serial.print(defense_v.x);
  Serial.print(",");
  Serial.print(defense_v.y);
  Serial.print(")  ");

  Serial.print(", Ball_dis=");
  Serial.print(ball_dis);

  Serial.print("Line(");
  Serial.print(line_angle);
  Serial.print(",");
  Serial.print(line_dis);


  Serial.print(",   leaveMode:");
  Serial.print(leave_mode);


  // Serial.print(", ball_speed:");
  // Serial.print(ball_speed);
  // Serial.print("Velocity(");
  // Serial.print(data.dp.velocity_v.x);
  // Serial.print(",");
  // Serial.print(data.dp.velocity_v.y);
  // Serial.print(")");
  // Serial.print(", N=");
  // Serial.print(echo.N); Serial.print(" ,NE=");
  // Serial.print(echo.NE); Serial.print(" ,E=");
  // Serial.print(echo.E); Serial.print(" ,SE=");
  // Serial.print(echo.SE); Serial.print(" ,S=");
  // Serial.print(echo.S); Serial.print(" ,SW=");
  // Serial.print(echo.SW);Serial.print(" ,W=");
  // Serial.print(echo.W); Serial.print(" ,NW=");
  // Serial.print(echo.NW); Serial.print(",");
  // Serial.print(",   powers=");
  // Serial.print(echo.central_power); Serial.print(",");
  // Serial.print(echo.righter_power); Serial.print(",");
  // Serial.print(echo.lefter_power); 

  // Serial.print(", leave:");
  // Serial.print(echo.leaveLineV(leave_mode).x);Serial.print(",");Serial.print(echo.leaveLineV(leave_mode).y);
  // Serial.print(")");
  // Serial.print("leave count:");
  // Serial.print(leave_count);
}

void Defense::debugLED(){
  //G,R,B
  switch(state){
      case OFF://0
        LED_color = pixel.Color(0, 0, 0);
        break;
      case line_tracing://1
        //ライントレース中：緑
        LED_color = pixel.Color(255, 0, 0);
        break;
      case lost_line://2
        //ロストライン：黄色
        LED_color = pixel.Color(255, 255, 0);
        break;
        //ダッシュ：青
      case keeper_dash://3
        LED_color = pixel.Color(0, 0, 255);
        break;
        //カメラ復帰：紫
      case back_to_goal_withCAM://4
        LED_color = pixel.Color(0, 255, 255);
        break;
        //超音波復帰:赤
      case back_to_goal_withECHO://5
        LED_color = pixel.Color(0, 255, 0);
        break;
        //脱出：白
      case leave_line://6
        LED_color = pixel.Color(255, 255, 255);
        break;
        //セルフトレース：青緑
      case self_trace://7
        LED_color = pixel.Color(150, 0, 200);
        break;
      //キーパーダッシュ用leave：弱め青
      case leave_dash://8
        LED_color = pixel.Color(0, 0, 150);
        break;
  }
  pixel.setPixelColor(0, LED_color);//G,R,B
  pixel.show();
}


//初期化、キーパー基礎関数、デバッグ/////////////////////////////////////////////////////////////////////////////
void Defense::LED_init(){
  pixel.begin();
  pixel.clear();
  ledOff();
}

void Defense::init(){
  LED_init();
  Serial.begin(115200);
}

void Defense::setBehavior(State s){
  state = s;
}

void Defense::ledOff(){
  LED_color = pixel.Color(0, 0, 0);
  pixel.setPixelColor(0, LED_color);//G,R,B
  pixel.show();
}

void Defense::off(){
  ledOff();
}

//アップデート/////////////////////////////////////////////////////////////////////////////
void Defense::update(){
  updateAllData();
  switch(state){
      case OFF:
        off();
        break;
      case line_tracing:
        lineTrace();
        break;
      case lost_line:
        lineTrace();
        break;
      // case keeper_dash:
      //   dash();
      //   break;
      case back_to_goal_withCAM:
        lineTrace();
        break;
      case back_to_goal_withECHO:
        lineTrace();
        break;
      // case leave_line:
      //   leaveLine(leave_mode);
      //   break;
      case self_trace:
        lineTrace();
        break;
      // case leave_dash:
      //   leaveDash();
      //   break;
  }
  if(state!=OFF && use_debug){
    debugSerial();
    debugLED();
  }
}

void Echo::update(){
    //移動平均
    for(int i=0; i<8; i++){
      if(data.dp.echoValues[i] != -1){
       ave[i] = ave[i]*(1 - new_data_ratio) + data.dp.echoValues[i]*new_data_ratio;
      }
    }
    N   = ave[0];
    NE  = ave[1];
    E   = ave[2];
    SE  = ave[3];
    S   = ave[4];
    SW  = ave[5];
    W   = ave[6];
    NW  = ave[7];   
}

void Defense::updateMPU(){
  angleZ = data.dp.robot_angle;
}

void Defense::updateLine(){
  if(data.dp.line_angle != 500){
    line_angle = data.dp.line_angle;
  }
  line_dis   = data.dp.line_distance;
  is_on_yoko = (abs(line_angle) < 20 || abs(line_angle)>170);
  is_on_tate = (abs(line_angle) > 70 && abs(line_angle) < 110);
}

void Defense::updateBall(){
  if(data.dp.ball_angle != 400){
    ball_angle = convertAngle(data.dp.ball_angle);
  }
  else{
    ball_angle = data.dp.ball_angle;
  }
  ball_dis   = data.dp.ball_distance;
  ball_speed = getBallD();
  if(ball_speed > ball_speed_max){
    ball_speed = ball_speed_max;
  }
}

void Defense::updateCam(){
//黄色を守り、青に攻める
  if(data.dp.is_attack_to_blue == true){
    cam_angle       = data.dp.yellow_angle;
    cam_dis         = data.dp.yellow_distance;
    cam_atack_angle = data.dp.blue_angle;
    cam_atack_dis   = data.dp.blue_distance;
  }
  //青を守り、黄色に攻める
  else{
    cam_angle       = data.dp.blue_angle;
    cam_dis         = data.dp.blue_distance;
    cam_atack_angle = data.dp.yellow_angle;
    cam_atack_dis   = data.dp.yellow_distance;
  }
  if(cam_on == false){
//超音波のみでデバッグするとき用↓
    cam_angle       = 400;
    cam_atack_angle = 400;
    cam_dis         = 0;
    cam_atack_dis   = 0;
  }
}

void Defense::updateAllData(){
  updateMPU();
  updateLine();
  echo.update();
  updateBall();
  updateCam();    
}

//ライントレース系//////////////////////////////////////////////////////////////////////////
float  Defense::getBallD(){
  uint32_t now = millis();
  uint32_t dt_ms = now - last_time;
  if (dt_ms > BALL_D_INTERVAL) {
    float dt = dt_ms / 1000.0f;
    ball_d = (convertAngle(ball_angle) - last_ball_angle) / dt;
    last_time = now;
    last_ball_angle = convertAngle(ball_angle);
  }
  ball_d = ball_d * 0.4 + last_ball_d * 0.6;
  last_ball_d = ball_d;
  return ball_d;
}

Vector Defense::ballV(){
  Vector v = {0,0};
  float ball_angle360 = reverseAngle(ball_angle);
  float line_angle360 = reverseAngle(line_angle);
  float ballV_angle;
  if(ball_angle != 400){
    //ボールをただ追うベクトル
    v = makeV(reverseAngle(ball_angle), ballV_speed);
    if(ball_angle > 0){
      ballV_angle = wrap360(line_angle360-90);
    }else{
      ballV_angle = wrap360(line_angle360+90);
    }
    //ライントレースする成分を抽出
    float power = lenV(v, ballV_angle); 
    v = makeV(ballV_angle,power);
    //速度を足す
    if(use_ball_speed == true && abs(ball_angle) < 90){
      v.x += ball_speed;
      // if(v.x < 0){
      //   v.x -= abs(ball_speed);
      // }
      // else{
      //   v.x += abs(ball_speed);
      // }
    } 
  }
  return v;
}

Vector Defense::lineV(){
  Vector v = {0,0};
  float power = line_dis * line_kp;
  if(line_dis < no_kp_dis){
    power = 0.0f;
  }
  v = makeV(reverseAngle(line_angle),power);
  last_line = makeV(reverseAngle(line_angle),lost_kp*100);
  return v;
}

Vector Defense::TlineV(Vector v){
  if(line_angle == 500){
    v.x = 0;
    v.y = (v.y < 0)? (v.y * -1 * T_kp): v.y;
  }
  return v;
}

Vector Defense::notToOwnGoal(){
  Vector v = {0,0};
  Vector traceV;
  // if(ball_angle > 0){
  //   trace = makeV(0,180);
  // }
  // else if(ball_angle < 0){
  //   trace = makeV(180,180);
  // }
  traceV =  changeLenV(ballV(), 100);
  v = addV(lineV(),traceV);
  return v;
}

Vector Echo::backWallBlockV(Vector v){
  if(S < before_T_s && v.y < 0){
    v.y = 0;
  }
  return v;
}
//セルフトレース/////////////////////////////////////////////////////////////////////////
void Defense::lineTrace(){
  Vector v = {0,0};
  if(echo.W < 400){
    v.x = 500;
  }
  else if(echo.E < 400){
    v.x = -500;
  }
  else if(ball_angle != 400){
    v.x = makeV(reverseAngle(ball_angle),pwm_max).x ;
    if(ball_angle > 0){
      v.x+= getBallD();
    }
    else{
      v.x-= getBallD(100);
    }
  }


  v.y = 1.2 * (echo.goal_area_s - echo.S);
  defense_v = v;
  data.dp.main_v = defense_v;
  defense_rotate = gyro.controlAngle(0);
  data.dp.main_rotate = defense_rotate;
}

//カメラ依存
Vector Defense::selfTraceV(){
  Vector v = {0,0};
  if(cam_angle != 400){
    //ゴールが右側にある
    if(cam_angle > self_on_angle){
      v = makeV(135,self_trace_speed);
    }
    //ゴールが左側にある
    else if(cam_angle < -1 * self_on_angle){
      v = makeV(45,self_trace_speed);
    }
    //中央にいると判断
    else{
      v = {0,0};
    }
  }
  return v;
}

void Defense::selfTrace(){
  Vector v = {0,0};
    defense_v = v;
    data.dp.main_v = defense_v;
    defense_rotate = gyro.controlAngle(0);          
    data.dp.main_rotate = defense_rotate; 
  
}

void Defense::lostLine(){
  Vector v = {0,0};
  if(abs(echo.S - echo.goal_area_s) < 40){
    state = line_tracing;
  }
  //最後の白線へ
  else{
     v = last_line;//lineV()内で更新
    lost_count++;
    //復帰した瞬間の回転待ち
    if(abs(angleZ) > 90){
      v = makeV(reverseAngle(0),0);    
    }
  }
  defense_v = v;
  data.dp.main_v = defense_v;
  defense_rotate = gyro.controlAngle(0);
  data.dp.main_rotate = defense_rotate;
}

//復帰/////////////////////////////////////////////////////////////////////////////
Vector Defense::lostGoalEchoV(){
  Vector v = {0,0};
  v.y = -1 * lost_echo_speed;
  int16_t quater_w = echo.coat_w * 0.5;
  if(echo.E < quater_w && echo.W > echo.coat_w){
    v.x = -1 * defense.lost_echo_speed * 1.5;
  }
  else if(echo.W < quater_w && echo.E > echo.coat_w){
    v.x = lost_echo_speed * 1.5;
  }
  else{
    v.x = 0;
  }
  return v;
}
void Defense::lostGoalEcho(){
  Vector v = {0,0};
  if(line_angle != 400){
    state = line_tracing;
  }
  else if(cam_angle != 400){
    state = back_to_goal_withCAM;
  }
  else{
    v = lostGoalEchoV();
  }
  defense_v = v;
  data.dp.main_v = defense_v;
  defense_rotate = gyro.controlAngle(0);
  data.dp.main_rotate = defense_rotate;
}

Vector Defense::cameraV(){
  Vector v = {0,0};
  float angle = wrap180(wrap180(cam_angle*cameraV_angle_kp)+180);
  if(cam_angle != 400){
    v = makeV(reverseAngle(angle),cameraV_speed); 
  }
  return v;
}

void Defense::lostGoalCamera(){
  Vector v = {0,0};
  if(line_angle != 400){
    state = line_tracing;
  }
  else if(cam_angle == 400){
    state = back_to_goal_withECHO;
  }
  else{
    v = cameraV();
  }
}



void Defense::finishKeeperDash(){
  state = back_to_goal_withCAM;
  attack.setBehavior(Attack::OFF);
}

//脱出とダッシュの判断///////////////////////////////////////////////////////////////////////
uint8_t Defense::isNeedTo_dash(){
  bool N_ok = (echo.N > 400);
  bool NE_ok = (echo.NE > 250);
  bool NW_ok = (echo.NW > 250);  
  bool angle_ok = abs(ball_angle) < 40;
  bool dis_ok = ball_dis >= 4;
  if(angle_ok && dis_ok && N_ok && NW_ok && NE_ok && on_dash){
    return 1;
  }
  else{
    return 0;
  }
}
uint8_t Defense::isNeedTo_leave(){
  return 0;
}