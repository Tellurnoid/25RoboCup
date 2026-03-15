#include <Arduino.h>
#include "Instance.h"
#include "Defense.h"

    void Echo::update(){
        N   = data.dp.echoValues[0];
        NE  = data.dp.echoValues[1];
        E   = data.dp.echoValues[2];
        SE  = data.dp.echoValues[3];
        S   = data.dp.echoValues[4];
        SW  = data.dp.echoValues[5];
        W   = data.dp.echoValues[6];
        NW  = data.dp.echoValues[7];      
    }

    Vector Echo::lostGoalEchoV(){
        Vector v;
        v = makeV(reverseAngle(180),defense.pwm_max*0.7);//超音波実装したら書く///////////////////
        return v;
    }

void Defense::setBehavior(State s){
  state = s;
}
void Defense::init(){
  Serial.begin(115200);
}
void Defense::debugSerial(){
  Serial.println(state);
}
void Defense::off(){}

void Defense::updateLine(){
    line_angle = data.dp.line_angle;
    line_dis   = data.dp.line_distance;
}
void Defense::updateBall(){
    if(data.dp.ball_angle != 400){
      ball_angle = convertAngle(data.dp.ball_angle);
    }
    else{
      ball_angle = data.dp.ball_angle;
    }
    ball_dis   = data.dp.ball_distance;
}
void Defense::updateCam(){
    //でき次第書く////////////////////////////////////////
}
//angleZ更新用
void Defense::updateMPU(){
    angleZ = data.dp.robot_angle;
}
void Defense::updateAllData(){
    echo.update();
    updateLine();
    updateBall();
    updateCam();    
    updateMPU();
}

void Defense::update(){
          updateAllData();
          
          switch(state){
              case out_of_running:
                off();
                break;
              case line_tracing:
                lineTrace();
                break;
              case lost_line:
                lostLine();
                break;
              case keeper_dash:
                dash();
                break;
              case back_to_goal_withCAM:
                lostGoalCamera();
                break;
              case back_to_goal_withECHO:
                lostGoalEcho();
                break;
          }
        if(state!=out_of_running){
          debugSerial();
        }
}

Vector Defense::ballV(){
    Vector v = makeV(reverseAngle(0),0);
    if(ball_angle != 400){
      //ボールを最速で追うベクトル
      v = makeV(reverseAngle(ball_angle),pwm_max);
      //その絶対値
      float ball_angle360 = reverseAngle(ball_angle);
      float line_angle360 = reverseAngle(line_angle);
      float ballV_angle;
      
      if(ball_angle > 0){
        ballV_angle = wrap360(line_angle360-90);
      }
      else{
        ballV_angle = wrap360(line_angle360+90);
      }
      // //白線が前にあるとき
      // if(abs(line_angle) < 90){
      //  ballV_angle = wrap360(line_angle360+180);
      // }
      // else{
      //  ballV_angle = wrap360(line_angle360);
      // }
      float power = lenV(v, ballV_angle); 
      v = makeV(ballV_angle,power);
    }
      // defense_v = v;
      // data.dp.main_v = defense_v;
      // defense_rotate = gyro.controlAngle(0);
      // data.dp.main_rotate = defense_rotate;
      return v;
  }

Vector Defense::lineV(){
    Vector v = makeV(reverseAngle(0),0);
    float power = line_dis * line_kp;
    float angle = line_angle;
    v = makeV(reverseAngle(line_angle),power);
      // defense_v = v;
      //     data.dp.main_v = defense_v;
      //     defense_rotate = gyro.controlAngle(0);
      //     data.dp.main_rotate = defense_rotate;
      return v;
}

Vector Defense::TlineV(Vector v){
    keeperDash_count = 0;
    v.x = 0;
    if(v.y<0)
      v.y = -1.5 * v.y;
      // defense_v = v;
      //     data.dp.main_v = defense_v;
      //     defense_rotate = gyro.controlAngle(0);          data.dp.main_rotate = defense_rotate;
      return v;
}

Vector Defense::notToOwnGoal(Vector v){
    //ライントレース中ボールが背後にあったら8割の速度にする
    if(line_angle != 400 && abs(ball_angle) > 90){
      v.x *= 0.8;
      v.y *= 0.8;
    }
  return v;
  }

void Defense::lineTrace(){
    Vector v = makeV(reverseAngle(0),0);
    if(line_angle == 400){
      state = lost_line;
    }
    else
    {
      v = assembleV(lineV(),ballV(),pwm_max);
      v = notToOwnGoal(v);
      if(line_angle == 500)
        v = TlineV(v);
    }
        defense_v = v;
        data.dp.main_v = defense_v;
        defense_rotate = gyro.controlAngle(0);
        data.dp.main_rotate = defense_rotate;
  }

void Defense::lostGoalCamera(){
    Vector v = makeV(reverseAngle(0),0);
    if(line_angle !=400){
      state = line_tracing;
    }
    if(c_x == 400){
      state = back_to_goal_withECHO;
    }
    //全包囲カメラつけたら書く////////////////////////////////////////////
    else{

    }
      defense_v = v;
      data.dp.main_v = defense_v;
      defense_rotate = gyro.controlAngle(0);     
      data.dp.main_rotate = defense_rotate; 
}
    void Defense::lostGoalEcho(){
      Vector v = {0,0};
      if(line_angle != 400){
        state = line_tracing;
      }
      else{
        v = echo.lostGoalEchoV();
      }
        defense_v = v;
        data.dp.main_v = defense_v;
        defense_rotate = gyro.controlAngle(0);
        data.dp.main_rotate = defense_rotate;
    }
void Defense::lostLine(){
    Vector v = makeV(reverseAngle(0),0);
    keeperDash_count = 0;
    //復帰した瞬間の回転待ち
    if(abs(angleZ) > 90){
      v = makeV(reverseAngle(0),0);
      
    }
    //ラインを見失って一定時間経過後、カメラで戻る
    if(lost_count >= 400){
      sound.single();
      state = back_to_goal_withCAM;
    }
    lost_count++;
    v = last_line;//lineV()内で更新
      defense_v = v;
      data.dp.main_v = defense_v;
      defense_rotate = gyro.controlAngle(0);
      data.dp.main_rotate = defense_rotate;
}

void Defense::dash(){
    Vector v = makeV(reverseAngle(0),0);
    //アタッカーのコードを
    //ここで呼び出し　or　変数切り替えてmain.inoで呼び出し

/////////EXPO時のもの↓

          //前に白線がある場合は除外、後ろの超音波見てボール追う
          bool is_line_front = (abs(line_angle) < 45)       ? true : false;
          bool keep_dashing = (is_line_front || line_angle) ? true : false;
          if(keep_dashing && echo.W < 400){
            v = ballV();
          }
          
          //タッチライン踏んだときの処理
          bool is_line_side = (abs(line_angle) > 30 && abs(line_angle) < 140) ? true : false;
          if(is_line_side){
            //右
            if(line_angle > 0){
              v.x = v.x * -1 - 500;
            }
            //左
            else{
              v.x = v.x * -1 + 500;
            }
          }
            defense_v = v;
          data.dp.main_v = defense_v;
          defense_rotate = gyro.controlAngle(0);          
          data.dp.main_rotate = defense_rotate;
}