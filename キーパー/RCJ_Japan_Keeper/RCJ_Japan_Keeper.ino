#include "UART.h"
#include "IMU.h"
#include "functions.h"

struct Sound{
    const int beep_pin = 21;
    void init(){
        pinMode(beep_pin, OUTPUT);
    }
    void success(){
      digitalWrite(beep_pin, HIGH);
      delay(50);
      digitalWrite(beep_pin, LOW);
      delay(50);
      digitalWrite(beep_pin, HIGH);
      delay(30);
      digitalWrite(beep_pin, LOW);      
    }
    void beep(){
       digitalWrite(beep_pin, HIGH);
       delay(50);
       digitalWrite(beep_pin, LOW);
    }
};

struct AngleOperation{
  float to360(float theta){
    if(theta < 0){
      theta = 360 + theta;
    }
    return theta;
  }
  
  float to180(float theta){
     if(theta > 180){
       theta = theta - 360;
     }
     return theta;
   }

  float normalization360(float theta){
    if(theta > 360){
      theta = theta - 360;
    }
    else if(theta < 0){
      theta = theta + 360;
    }
    return theta;
  }

  float normalization180(float theta){//角度計算後の正規化
    if(theta > 180){
      theta = -1  * (360 - theta);
    }
    else if(theta < -180){
      theta = 360 - abs(theta);
    }
    return theta;
  }
};

struct Echo{
    Vector lostGoalEchoV(){
        Vector v;
        v = makeV(0,0);
        return v;
    }
};

struct Keeper{
  //EchoとSoundに依存
  Echo echo;
  AngleOperation angle;
  Sound sound;
  //打ち消し
  float ball_angleL;//360度の方式
  float remove_angle;
  float remove_power;//打ち消し

  //ライントレース
  float lineDf_dis;
  float lineDf_angle;
  Derivative lineD_dis;
  Derivative lineD_angle;
  bool isOnCurve = false;
  float last_angle;

  //カメラ(line_angle==400時)
  uint16_t c_x_max = 158;//コートの横端から見たゴール
  uint8_t move_power_cam = 0;
  uint8_t move_angle_cam = 0;

  //超音波(line_angle==400時)
  uint8_t coat_w_forPower = 70;//比例定数、切片用
  uint8_t coat_h =  67;
  uint8_t coat_h_forPower =  38;
  uint8_t coat_b = 20;

  //キーパーダッシュ
  int half_coat = 80;//170/2
  uint16_t lost_count = 0;

  //全体で使う
  uint16_t keeperDash_count = 0;
  Vector last_line;

  enum State{
    out_of_running,
    calibrating,
    line_tracing,
    lost_line,
    keeper_dash,
    back_to_goal_withCAM,
    back_to_goal_withECHO,
  };
  State state = out_of_running;

  //モーターが動く最大値と最小値
  float PercentToPWM(float percent = 100,float PWM_min=70,float PWM_max=255){//百分率をPWMに変換(最大値、最小値は調整してください)
      return PWM_min + (PWM_max - PWM_min) * percent / 100;
  }

  Vector cameraBrakeV(){
    Vector v;
    v.x = 0;

    return v;
  }

  Vector removeV(){

  }

  Vector ballV(){

  }

  Vector lineV(){

  }

  Vector TlineV(){
    Vector v;
    keeperDash_count = 0;
    v.y = 0;
    if(v.x<0)
      v.x = -1.5 * v.x;
    return v;
  }
  Vector notToOwnGoal(Vector v){

  }

  Vector lineTraceV(){//ライン検出時、キーパーの動き
    Vector v;
    //ライン見えなくなったらすぐreturn
    if(line_angle == 400){
        state = lost_line;
        return;
    }
    //T字 
    else if(line_angle == 500){
        v = TlineV();
        return v;
    }
    //ライントレース
    else{
       v = ballV();
       v = addV(v, lineV());
       v = addV(v, removeV());
       bool is_sliding = (abs(line_angle) < 30 || abs(line_angle) > 140);
       if(is_sliding){
           v = addV(v, cameraBrakeV());
       }
       v = notToOwnGoal(v);
       return v;
    }
    
    //キーパーダッシュを検討する条件
    bool is_ball_free = (abs(v.y) < 110 && ball_dis >= 3);
    if(is_ball_free){
        keeperDash_count++;
    }
    else{
        keeperDash_count = 0;
    }

    if(keeperDash_count > 100){
      state =  keeper_dash;
      return;
    }
  }

  //カメラを使って戻る動き
  Vector lostGoalCameraV(){
    Vector v = makeV(0,0);
    //ゴールを検出できなければ超音波に切り替え
    if(c_x == 400){
        state = back_to_goal_withECHO;
        return;
    }
    //正面とみなして、後ろにまっすぐ戻る
    else if(abs(c_x) < 30){
        v = makeV(180,150);
        return v;
    }
    else if(c_x > 0){
      move_power_cam = c_x * 100/c_x_max;
      if(c_x_max < c_x){move_power_cam = 100;}
      v.y = PercentToPWM(move_power_cam,60,180);
    }
    else{
      move_power_cam = -1 * c_x * 100/c_x_max;
      if(c_x_max < -1 * c_x){move_power_cam = 100;}
      v.y = -1 * PercentToPWM(move_power_cam,60,180);    
    }

  return v;
  }

  //ライン未検出時、lastlineに戻る動き
  Vector lostLineV(){
    Vector v;
    keeperDash_count = 0;
    //復帰した瞬間の回転待ち
    if(abs(angleZ) > 90){
      v = makeV(0,0);
    }
    //ラインを見失って一定時間経過後、カメラで戻る
    if(lost_count >= 400){
        sound.beep();
        state = back_to_goal_withCAM;
        return;
    }
    //横移動でラインアウトしてしまった場合もカメラで戻る(旧機体は必須)
    bool is_over_run = (abs(c_x) > 90 && v.x < 0);//c_xのしきい値要調整
    if(is_over_run){
        state = back_to_goal_withCAM;
        return;
    }
    lost_count++;
    v = last_line;//lineV()内で更新
    return v;
  }


  Vector DashV(){}

  void main(){
    Vector v;
    switch(state){
        case out_of_running:
          v = makeV(0,0);
          break;
        case calibrating:
          v = makeV(0,0);
          break;
        case line_tracing:
          v = lineTraceV();
        case lost_line:
          v = lostLineV();
          break;
        case keeper_dash:
          v = DashV();
          break;
        case back_to_goal_withCAM:
          v = lostGoalCameraV();
          break;
        case back_to_goal_withECHO:
          v = echo.lostGoalEchoV();
          break;
    }
    moveVector(v, rotatePID(0, 0));
  }

  void debugSerial(){
    
  }
};


Sound sound;
Echo echo;
Keeper keeper;
void setup(){
    sound.init();
    initUART();
    initIMU();
    initMotor();
}

void loop(){
    IMU();
    UART();
    keeper.main();
}