#include <Arduino.h>
#include "Instance.h"
#include "Defense.h"
#include <Adafruit_NeoPixel.h>
    //多分使わないsound.single()
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



void Echo::convertLine_sideOrBottom(){
     //おしりトレース用  (lineV()でのlast_lineも要確認)
    if(bottom_trace && defense.is_on_yoko && defense.line_angle != 400){
      //背後に白線あり
      if(abs(defense.line_angle) > 90){
          //しきい値より前
          if(defense.line_dis < bottom_trace_dis){
            defense.line_angle = wrap180(defense.line_angle + 180);
            defense.line_dis = abs(defense.line_dis - bottom_trace_dis);
          }
          //しきい値より後ろ
          else{
            defense.line_dis = bottom_trace_kp * abs(defense.line_dis - bottom_trace_dis);
          } 
      }
      //前に白線あり
      else{
        defense.line_dis = defense.line_dis + bottom_trace_dis;
        if(defense.line_dis > 100){
          defense.line_dis = 100;
        }
      }
    }

     //横端トレース用  (lineV()でのlast_lineも要確認)
    else if(defense.line_angle != 400 && bottom_trace == false && side_trace != 0){
        //壁側に白線
        if((defense.line_angle > 0 && side_trace == 1) || (defense.line_angle < 0 && side_trace == 2)){
            //しきい値より前
            if(defense.line_dis < side_trace_dis){
              defense.line_angle = wrap180(defense.line_angle + 180);
              defense.line_dis = abs(defense.line_dis - side_trace_dis);
            }
            //しきい値より後ろ
            else{
              defense.line_dis = side_trace_kp * abs(defense.line_dis - side_trace_dis);
            } 
        }
        //壁の逆側に白線
        else{
            defense.line_dis = defense.line_dis + side_trace_dis;
            if(defense.line_dis > 100){
              defense.line_dis = 100;
            }
        }
      }
}

void Echo::updateLineOrBottom(){
        //後ろ近ければおしりトレース有効化
        if(S < wall_S){
          bottom_trace = true;
          side_trace   = 0;
          defense.line_kp = bottom_trace_kp;
          convertLine_sideOrBottom();
        }
        //右が近ければサイドトレース有効化
        else if(E < wall_side){
          bottom_trace = false;
          side_trace   = 1;
          defense.line_kp = side_trace_kp;
          convertLine_sideOrBottom();
        }
        //左が近ければサイドトレース有効化
        else if(W < wall_side){
          bottom_trace = false;
          side_trace   = 2;
          defense.line_kp = side_trace_kp;
          convertLine_sideOrBottom();
        }
        else{
          bottom_trace = false;
          side_trace = 0;
          defense.line_kp = defense.line_default_kp;
        }
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
  // pixels.begin();
  // pixels.clear();
  Serial.begin(115200);
}

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
      Serial.print(", echo_dif=");
      Serial.print(echo.echo_dif);


      Serial.print("Velocity(");
      Serial.print(data.dp.velocity_v.x);
      Serial.print(",");
      Serial.print(data.dp.velocity_v.y);
      Serial.print(")");
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
  if(line_angle != 400){
    
  }
  Serial.println();
}


void Defense::off(){}

void Defense::updateVelocity(){
    vel_x = data.dp.velocity_v.x;
    vel_y = data.dp.velocity_v.y;
    vel_power = sqrt(vel_x * vel_x + vel_y * vel_y);
  }

void Defense::updateLine(){
    line_angle = data.dp.line_angle;
    line_dis   = data.dp.line_distance;
     is_on_yoko = (abs(line_angle) < 20 || abs(line_angle)>170);
     is_on_tate = (abs(line_angle) > 70 && abs(line_angle) < 110);
     echo.updateLineOrBottom();
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
    //黄色を守り、青に攻める
     if(is_atack_to_BLUE == true){
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
    //超音波のみでデバッグするとき用↓
    //  cam_angle = 400;
    //  cam_atack_angle = 400;
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
      updateVelocity();
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

          //おしりトレースで見失ったときの減速対策
    if(echo.bottom_trace && is_on_yoko && abs(line_angle) > 90){
      last_line = makeV(reverseAngle(line_angle),echo.lost_bottom_kp * vel_power);
    }
    //サイドトレースで見失ったときの減速対策
    if(echo.side_trace != 0 && is_on_yoko && abs(line_angle) > 90){
      last_line = makeV(reverseAngle(line_angle),echo.lost_side_kp * vel_power);
    }
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
    //last_line = makeV(reverseAngle(line_angle),line_kp*100);
    //機体速度に応じるやつ↓
    last_line = makeV(reverseAngle(line_angle),lost_line_kp * vel_power);

    // //おしりトレースで見失ったときの減速対策
    // if(echo.bottom_trace && is_on_yoko && abs(line_angle) > 90){
    //   last_line = makeV(reverseAngle(line_angle),echo.lost_bottom_kp * vel_power);
    // }
    // //サイドトレースで見失ったときの減速対策
    // if(echo.side_trace != 0 && is_on_yoko && abs(line_angle) > 90){
    //   last_line = makeV(reverseAngle(line_angle),echo.lost_side_kp * vel_power);
    // }
      return v;
}

Vector Defense::TlineV(Vector v){
      if(line_angle == 500){
        v.x = 0;
        if(v.y < 0){
           v.y *= -1;
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
    //ラインが見えない
    if(line_angle == 400){
      state = lost_line;
    }
    //ボール見えないとき中央に向かう
    else if(ball_angle == 400){
        lost_count = 0;
        state = self_trace;
    }
    //T字
    else if(isNeedToLeave()==500){
        lost_count = 0;
        v = assembleV(lineV(),ballV(),pwm_max);
        v = TlineV(v);
        v = echo.backWallBlockV(v);
    }
    //脱出
    else if(isNeedToLeave()==1){
        state = leave_line;
    }
    //ラインもボールも認識中   通常のライントレース
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
      defense_v = v;
      data.dp.main_v = defense_v;
      defense_rotate = gyro.controlAngle(0);
      data.dp.main_rotate = defense_rotate;
}

void Defense::lostLine(){
    Vector v = makeV(reverseAngle(0),0);
    keeperDash_count = 0;
    if(line_angle != 400){
      state = line_tracing;
    }
    //ラインを見失って一定時間経過後、カメラで戻る
    else if(lost_count >= 400){
      state = back_to_goal_withCAM;
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



Vector Defense::cameraV(){
  Vector v;
  v.x = makeV(reverseAngle(cam_angle),pwm_max).x;
  v.y = echo.goal_area_s - echo.S;

  //横軸の安全対策
  if(abs(v.x) > 300){
    if(v.x >= 0)
      v.x = 300;
    if(v.x < 0)
      v.x = -300;
  }
  //後退時の最小値
  if(echo.S == -1 || (v.y < 0 && abs(v.y) < 200)){
    v.y = -200;
  }
  //後ろに壁があって、前進するときの最大値
  if(v.y > 0 && abs(v.x) < 200){
    v.y = 200;
  }
  return v;
}

void Defense::lostGoalCamera(){
    Vector v = makeV(reverseAngle(0),0);
    //カメラ認識できなければ超音波
    if(cam_angle == 400){
      state = back_to_goal_withECHO;
    }
    //ライン踏んだら
    else if(line_angle != 400){
        if(isNeedToLeave()==500){
          state = line_tracing;
        }
        else if(isNeedToLeave()==1){
          state = leave_line;
        }
        else{
          state = line_tracing;
        }
    }
    //カメラ適用、後退速度は超音波使用
    else{
       v = cameraV();
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
    v.y = -1 * abs(speed_back);


  if(E != -1 && W != -1){
      //遮蔽物なしで横幅の計測成功したとき
      if(E + W < 700){
        v.x = 0;
      }
      else if(E < wall_w){
        v.x = (E - wall_w)*lost_goal_echo_kp; 
      }
      else if(W < wall_w){
        v.x = (wall_w - W)*lost_goal_echo_kp;
      }
      else{
        v.x = 0;
      }
      last_vx = v.x;
    }
    else{
      v.x = last_vx;
    }
  //横軸の安全対策
  if(abs(v.x) > 300){
    if(v.x >= 0)
      v.x = 300;
    if(v.x < 0)
      v.x = -300;
  }
  //後退時の最小値
  if(S == -1 || (v.y < 0 && abs(v.y) < 200)){
    v.y = -200;
  }
  //後ろに壁があって、前進するときの最大値
  if(v.y > 0 && abs(v.x) < 200){
    v.y = 200;
  }
    return v;
}
void Defense::lostGoalEcho(){
  Vector v = {0,0};
  //カメラ見えたら切り替える
  if(cam_angle != 400){
    state = back_to_goal_withCAM;
  }
  //ライン踏んだら
  else if(line_angle != 400){
    if(isNeedToLeave()==500){
      state = line_tracing;
    }
    else if(isNeedToLeave()==1){
      state = leave_line;
    }
    else{
      state = line_tracing;
    }
  }
  //超音波適用
  else{
    v = echo.lostGoalEchoV();
  }
    defense_v = v;
    data.dp.main_v = defense_v;
    defense_rotate = gyro.controlAngle(0);
    data.dp.main_rotate = defense_rotate;
}








uint16_t Defense::isNeedToLeave(){
  //攻めるべきゴールに近いか
  is_on_atack_goal = (cam_atack_angle != 400 && (cam_atack_dis > 0) && cam_atack_dis < cam_far_from_goal*2);
  
  if(line_angle == 500){
    leave_count = 0;
    return 500;
  }
  //横からの脱出判定はカウントが条件なので、白線出るまで続ける
  else if(state == leave_line && (leave_mode == 'W' || leave_mode == 'E') && line_angle != 400){
      return 1;
  }
  //ゴールラインのトレースを始めたとき
  else if(echo.S < echo.wall_S && echo.N > echo.goal_area_s*1.5 && is_on_yoko){
    leave_mode = 'S';
    state = leave_line;
    return 1;
  }
  //相手のゴール前でトレースを始めたとき // || (cam_far_from_goal && echo.N < echo.goal_area_n && echo.S > echo.goal_area_s*1.5)
  else if(is_on_atack_goal){
    leave_mode = 'N';
    state = leave_line;
    return 1;
  }
  //タッチライン(右)のトレースを始めたとき
  else if(cam_dis > cam_far_from_goal && echo.E < echo.wall_side && echo.W > echo.wall_side *1.5  && is_on_tate){
    leave_count++;
    if(leave_count > 150){
      leave_mode = 'E';
      state = leave_line;
      return 1;
    }
    else{
      return 0;
    }
  }
//タッチライン(左)のトレースを始めたとき
  else if(cam_dis > cam_far_from_goal && echo.W < echo.wall_side && echo.E > echo.wall_side * 1.5 && is_on_tate){
    leave_count++;
    if(leave_count > 150){
      leave_mode = 'W';
      state = leave_line;
      return 1;
    }
    else{
      return 0;
    }
  }
  else{
    leave_count = 0;
    return 0;
  }
}

  Vector Echo::leaveLineV(char direction){
    Vector v;
    if(direction == 'N'){
      //後方３つの超音波を見て動く
      central_power = leave_kp * abs((N  < S) ? echo_dif - N : 0);
      righter_power = leave_kp * abs((NE < SW)? echo_dif - NE: 0);
      lefter_power  = leave_kp * abs((NW < SE)? echo_dif - NW: 0);
      back_wall_power = 0;
      central_angle = 270;
      righter_angle = 225;
      lefter_angle  = 315;
      central_power = 600;////////////////////////////////////////////////////////////////
    }
    else if(direction == 'S'){
      central_power = leave_kp * abs((S  < N) ? echo_dif - S : 0);
      righter_power = leave_kp * abs((SW < NE)? echo_dif - SW: 0);
      lefter_power  = leave_kp * abs((SE < NW)? echo_dif - SE: 0);
     righter_power = 0;
     lefter_power = 0;

      back_wall_power = 0;
      central_angle = 90;
      righter_angle = 45;
      lefter_angle  = 135;
    }
    else if(direction == 'E'){
      central_power = leave_kp * abs((E  < W)  ? echo_dif - E : 0);
      righter_power = leave_kp * abs((SE < NW) ? echo_dif - SE: 0);
      lefter_power  = leave_kp * abs((NE < SW) ? echo_dif - NE: 0);
      back_wall_power = leave_kp * ((S < wall_S)? echo_dif - S: 0);
      central_angle = 180;
      righter_angle = 135;
      lefter_angle  = 225; 
    }
    else if(direction == 'W'){
      central_power = leave_kp * abs((W  < E)  ? echo_dif - W : 0);
      righter_power = leave_kp * abs((NW < SE) ? echo_dif - NW: 0);
      lefter_power  = leave_kp * abs((SW < NE) ? echo_dif - SW: 0);
      back_wall_power = leave_kp * ((S < wall_S)? echo_dif - S: 0);
      central_angle = 0;
      righter_angle = 215;
      lefter_angle  = 45;
    
    }
    central = makeV(central_angle, central_power);
    righter = makeV(righter_angle, righter_power);
    lefter  = makeV(lefter_angle,  lefter_power);
    // righter_angle = wrap360(central_angle + 45);
    // lefter_angle = wrap360(central_angle - 45);
    v = {
          central.x + righter.x + lefter.x,
          central.y + righter.y + lefter.y,
        };
   // v = central;
    return v;
  }

  //directionは進む方向ではなく壁の方向なので注意
  void Defense::leaveLine(char direction){
    Vector v;
    if(line_angle == 400){
      state = back_to_goal_withCAM;
    }
    else if(isNeedToLeave() == 0){
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







Vector Defense::selfTraceV(float percent){
  Vector v;
  float angle;
  if(ball_angle != 400){
    state = line_tracing;
  }
  //カメラ
  else if(cam_angle != 400){
    if(180 - abs(cam_angle) < 10){
      v = {0,0};
    }
    //ゴールが左側にある
    else if(cam_angle < 0){
      v = makeV(135,3*(180 - abs(cam_angle))+150);
    }
    //ゴールが右側にある
    else{
      v = makeV(45,3*(180 - abs(cam_angle))+150);
    }
  }
  // else if(echo.E == -1 || echo.W == -1){
  //   v = {0,0};
  // }
  else{
    if(echo.E < echo.W){
       //= (abs(line_angle) > 90)? angle = line_angle+90:angle = line_angle-90;
       v = makeV(135,200);
    }
    else{
      v = makeV(45,200);
    }
  }
  return v;
}

void Defense::selfTrace(){
  Vector v;
  //ボールが見えたとき、ライン見えないときは通常のライントレース
  if(ball_angle != 400 || line_angle == 400){
    state = line_tracing;
  }
//T字
  else if(isNeedToLeave()==500){
      lost_count = 0;
      v = assembleV(lineV(),ballV(),pwm_max);
      v = TlineV(v);
      v = echo.backWallBlockV(v);
  }
  //脱出
  else if(isNeedToLeave()==1){
      state = leave_line;
  }
  //selfTraceV適用
  else {
    defense_v = assembleV(lineV(),selfTraceV(40),pwm_max);
    data.dp.main_v = defense_v;
    defense_rotate = gyro.controlAngle(0);          
    data.dp.main_rotate = defense_rotate; 
  }
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