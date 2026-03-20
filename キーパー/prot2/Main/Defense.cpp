#include <Arduino.h>
#include "Instance.h"
#include "Defense.h"
  
    //多分使わない
    void Echo::calibrateW(){
      uint16_t count = 50;
      uint16_t sum = 0;
      for(uint16_t i=0; i<count; i++){
        uart.update();
        update();
        sum += (E + W)/2;
      }
      wall_w = sum/count;
    }

    void Echo::update(){
        for(int i=0; i<8; i++){
          ave[i] = ave[i]*(1 - new_data_ratio) + data.dp.echoValues[i]*new_data_ratio;
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

    Vector Echo::backWallBlockV(Vector v){
      if(S < wall_S && v.y < 0 && abs(defense.line_angle)==90)
        v.y *= -1.2;
      return v;
    }

    Vector Echo::withoutLineV(){
      Vector v;
        v.x = 0;
        v.y = defense.pwmDomain(goal_area_s - S + 100);
      
        v = addV(v,defense.ballV());
        if(E < wall_side + tolerance){
          v.x = -1 * defense.pwm_max;
        }
        else if(W < wall_side + tolerance){
          v.x = defense.pwm_max;
        }
      return v;
    }

//最大値、最小値の中に留める関数(404で無効化)
int Defense::pwmDomain(int val){
    int max = pwm_max;
    int min = pwm_max * -1;
    if(val < min){
        return min;
    }
    else if(val > max){
        return max;
    }
    else{
        return val;
    }
}

void Defense::setBehavior(State s){
  state = s;
}
void Defense::init(){
  Serial.begin(115200);
}
void Defense::debugSerial(){
  Serial.print("State:");
  Serial.print(state);
  Serial.print(" (");
  Serial.print(defense_v.x);
  Serial.print(",");
  Serial.print(defense_v.y);
  Serial.print(")  ");
  Serial.print(",   leaveMode:");
  Serial.print(leave_mode);
  Serial.print(", echo_dif=");
  Serial.print(echo.echo_dif);

Serial.print(", N=");
Serial.print(echo.N); Serial.print(" ,NE=");
Serial.print(echo.NE); Serial.print(" ,E=");
Serial.print(echo.E); Serial.print(" ,SE=");
Serial.print(echo.SE); Serial.print(" ,S=");
Serial.print(echo.S); Serial.print(" ,SW=");
Serial.print(echo.SW);Serial.print(" ,W=");
Serial.print(echo.W); Serial.print(" ,NW=");
Serial.print(echo.NW); Serial.print(",");
Serial.print(",   powers=");
Serial.print(echo.central_power); Serial.print(",");
Serial.print(echo.righter_power); Serial.print(",");
Serial.println(echo.lefter_power);


  Serial.println();
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
    cam_angle = data.dp.goal_angle;
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
              case leave_line:
                leaveLine(leave_mode);
                break;
              case self_trace:
                selfTrace();
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
Vector Defense::lineV_onlyP(){
    Vector v = makeV(reverseAngle(0),0);
    float power = line_dis * line_kp;
    float angle = line_angle;
    v = makeV(reverseAngle(line_angle),power);
    last_line = v;
      // defense_v = v;
      //     data.dp.main_v = defense_v;
      //     defense_rotate = gyro.controlAngle(0);
      //     data.dp.main_rotate = defense_rotate;
      return v;
}

Vector Defense::lineV(){
  //PID制御(tbc)
  // Vector v = makeV(reverseAngle(0),0);
  //   //比例
  //    line_P = line_dis * line_kp;

  //   //微分
  //   unsigned long now = millis();
  //   float dt = (now - prev_time) / 1000.0;
  //   line_D = line_kd * (line_dis - prev_line_dis) / dt;

  //   //積分
  //   line_I += line_ki * line_dis / dt;

  //   float power = line_P + line_I + line_D;
  //   if(power > pwm_max){
  //     power = pwm_max;
  //   }
  //   float angle = line_angle;
  //   v = makeV(reverseAngle(line_angle),power);
  //   lost_v = v;

  //   prev_line_dis = line_dis;
  //   return v;
    Vector v = makeV(reverseAngle(0),0);
    float power = line_dis * line_kp;
    float angle = line_angle;
    v = makeV(reverseAngle(line_angle),power);
    last_line = v;
      return v;
}

Vector Defense::TlineV(Vector v){
      if(line_angle == 500){
        v.x = 0;
        if(v.y < 0){
           v.y *= -1.5;
          }
      }
     return v;
}

Vector Defense::selfTraceV(float percent){
  Vector v;
  float angle;
  if(ball_angle != 400){
    state = line_tracing;
  }
  if(echo.E == -1 || echo.W == -1){
    v = {0,0};
  }
  else{
    if(echo.position_x > 0){
       //= (abs(line_angle) > 90)? angle = line_angle+90:angle = line_angle-90;
    }
  }
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
     is_on_yoko = (line_angle == 0 || abs(line_angle) == 180);
     is_on_tate = (abs(line_angle) == 90);
    //ラインが見えない
    if(line_angle == 400){
      v = last_line;
      lost_count++;
      if(lost_count > 200){
        state = lost_line;
      }
    }
      //ゴールの中央にゆっくり向かう
    else if(ball_angle == 400){
        lost_count = 0;
        state = self_trace;
    }
    else if(ball_angle == 500){
        lost_count = 0;
        v = assembleV(lineV(),ballV(),pwm_max);
        v = TlineV(v);
        v = echo.backWallBlockV(v);
    }
    else{
        //ゴールラインのトレースを始めたとき
        if(echo.S < echo.wall_S && echo.N > echo.goal_area_s*1.5 && is_on_yoko){
          leave_mode = 'S';
          lost_count = 0;
          sound.single();
          state = leave_line;
        }
        //相手のゴール前でトレースを始めたとき
        else if(echo.N < echo.goal_area_s && echo.S > echo.goal_area_s*1.5){
          leave_mode = 'N';
          lost_count = 0;
          sound.single();
          state = leave_line;
        }
        //タッチライン(右)のトレースを始めたとき
        else if(echo.E < echo.wall_side && echo.W > echo.wall_side *1.5  && is_on_tate){
          leave_mode = 'E';
          lost_count = 0;
          sound.single();
          state = leave_line;
        }
      //タッチライン(左)のトレースを始めたとき
        else if(echo.W < echo.wall_side && echo.E > echo.wall_side * 1.5 && is_on_tate){
          leave_mode = 'W';
          lost_count = 0;
          sound.single();
          state = leave_line;
        }
       //ラインもボールも認識中
        else{
        lost_count = 0;
        v = assembleV(lineV(),ballV(),pwm_max);
        //縦線上で後退するときは減速
        bool is_on_tate = (abs(line_angle) > 45 && abs(line_angle) < 135);
        if(is_on_tate && v.y < 0){
          v.x *= 0.3;
          v.y *= 0.3;
        }
        v = notToOwnGoal(v);
        v = TlineV(v);
        v = echo.backWallBlockV(v);
        }
    }
      defense_v = v;
      data.dp.main_v = defense_v;
      defense_rotate = gyro.controlAngle(0);
      data.dp.main_rotate = defense_rotate;
  }

void Defense::lostGoalCamera(){
    Vector v = makeV(reverseAngle(0),0);
    if(cam_angle == 400){
      state = back_to_goal_withECHO;
    }
    else if(line_angle != 400){
        if(line_angle==500){
          state = line_tracing;
        }
        //ゴールラインのトレースを始めたとき
        else if(echo.S < echo.wall_S && echo.N > echo.goal_area_s*1.5 && is_on_yoko){
          leave_mode = 'S';
          sound.single();
          state = leave_line;
        }
        //相手のゴール前でトレースを始めたとき
        else if(echo.N < echo.goal_area_s && echo.S > echo.goal_area_s*1.5){
          leave_mode = 'N';
          sound.single();
          state = leave_line;
        }
        //タッチライン(右)のトレースを始めたとき
        else if(echo.E < echo.wall_side && echo.W > echo.wall_side *1.5  && is_on_tate){
          leave_mode = 'E';
          sound.single();
          state = leave_line;
        }
      //タッチライン(左)のトレースを始めたとき
        else if(echo.W < echo.wall_side && echo.E > echo.wall_side * 1.5 && is_on_tate){
          leave_mode = 'W';
          sound.single();
          state = leave_line;
        }
        else{
          state = line_tracing;
        }
    }
    //全包囲カメラつけたら書く////////////////////////////////////////////
    else{
      v = makeV(reverseAngle(cam_angle),pwm_max);
      v.y = echo.goal_area_s - echo.S -100;
    }
      defense_v = v;
      data.dp.main_v = defense_v;
      defense_rotate = gyro.controlAngle(0);     
      data.dp.main_rotate = defense_rotate; 
}


    Vector Echo::lostGoalEchoV(){
        Vector v;
        float last_vx;
        //v.y = defense.pwmDomain(goal_area_s - S + 200);
        v.y = goal_area_s - S - 100;
        
        if(E != -1 && W != -1){
          //遮蔽物なしで横幅の計測成功したとき
          if(E + W < 700){
            v.x = 0;
          }
          else if(E < wall_w){
            v.x = (E - wall_w)*1.5; 
          }
          else if(W < wall_w){
            v.x = (wall_w - W)*1.5;
          }
          else{
            v.x = 0;
          }
          last_vx = v.x;
        }
        else{
          v.x = last_vx;
        }
        return v;
    }
    void Defense::lostGoalEcho(){
      Vector v = {0,0};
      if(cam_angle != 400){
        state = back_to_goal_withCAM;
      }
      else if(line_angle != 400){
        if(line_angle==500){
          state = line_tracing;
        }
        //ゴールラインのトレースを始めたとき
        else if(echo.S < echo.wall_S && echo.N > echo.goal_area_s*1.5 && is_on_yoko){
          leave_mode = 'S';
          sound.single();
          state = leave_line;
        }
        //相手のゴール前でトレースを始めたとき
        else if(echo.N < echo.goal_area_s && echo.S > echo.goal_area_s*1.5){
          leave_mode = 'N';
          sound.single();
          state = leave_line;
        }
        //タッチライン(右)のトレースを始めたとき
        else if(echo.E < echo.wall_side && echo.W > echo.wall_side *1.5  && is_on_tate){
          leave_mode = 'E';
          sound.single();
          state = leave_line;
        }
      //タッチライン(左)のトレースを始めたとき
        else if(echo.W < echo.wall_side && echo.E > echo.wall_side * 1.5 && is_on_tate){
          leave_mode = 'W';
          sound.single();
          state = leave_line;
        }
        else{
          state = line_tracing;
        }
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

  Vector Echo::leaveLineV(char direction){
    Vector v;
    // float kp = 3;
    // int echo_dif = 1000;
    // float central_power;
    // float central_angle;
    // float righter_power;
    // float righter_angle;
    // float lefter_power;
    // float lefter_angle;
    // Vector central = {0,0};
    // Vector righter = {0,0};
    // Vector lefter  = {0,0};

    if(direction == 'N'){
      //後方３つの超音波を見て動く
      central_power = leave_kp * abs((N  < S) ? echo_dif - N : 0);
      righter_power = leave_kp * abs((NE < SW)? echo_dif - NE: 0);
      lefter_power  = leave_kp * abs((NW < SE)? echo_dif - NW: 0);
      back_wall_power = 0;
      central_angle = 270;
      // righter_angle = 225;
      // lefter_angle  = 315;
    }
    else if(direction == 'S'){
      central_power = leave_kp * abs((S  < N) ? echo_dif - S : 0);
      righter_power = leave_kp * abs((SW < NE)? echo_dif - SW: 0);
      lefter_power  = leave_kp * abs((SE < NW)? echo_dif - SE: 0);
      back_wall_power = 0;
      central_angle = 90;
      // righter_angle = 45;
      // lefter_angle  = 135;
    }
    else if(direction == 'E'){
      central_power = leave_kp * abs((E  < W)  ? echo_dif - E : 0);
      righter_power = leave_kp * abs((SE < NW) ? echo_dif - SE: 0);
      lefter_power  = leave_kp * abs((NE < SW) ? echo_dif - NE: 0);
      back_wall_power = leave_kp * ((S < wall_S)? echo_dif - S: 0);
      central_angle = 180;
      // righter_angle = 135;
      // lefter_angle  = 225; 
    }
    else if(direction == 'W'){
      central_power = leave_kp * abs((W  < E)  ? echo_dif - W : 0);
      righter_power = leave_kp * abs((NW < SE) ? echo_dif - NW: 0);
      lefter_power  = leave_kp * abs((SW < NE) ? echo_dif - SW: 0);
      back_wall_power = leave_kp * ((S < wall_S)? echo_dif - S: 0);
      central_angle = 0;
      // righter_angle = 215;
      // lefter_angle  = 45;
    }
    righter_angle = wrap360(central_angle + 45);
    lefter_angle = wrap360(central_angle - 45);
    v = {
          central.x + righter.x + lefter.x + back_wall.x,
          central.y + righter.y + lefter.y + back_wall.y,
        };
    return v;
  }
  //directionは進む方向ではなく壁の方向なので注意
  void Defense::leaveLine(char direction){
    Vector v;
    if(line_angle == 400){
      state = back_to_goal_withCAM;
    }
    else if(line_angle == 500 && echo.S < echo.goal_area_s){
      state = line_tracing;
    }
    else{
      sound.single();
      v = echo.leaveLineV(direction);
      defense_v = v;
      data.dp.main_v = defense_v;
      defense_rotate = gyro.controlAngle(0);          
      data.dp.main_rotate = defense_rotate;
    }
  }

void Defense::selfTrace(){
      defense_v = assembleV(lineV(),selfTraceV(40),pwm_max);
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