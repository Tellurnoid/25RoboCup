#include <Arduino.h>
#include "Instance.h"
       
       void Keeper:init() {
          gyro.initGyro();
          uart.initUART();
          sound.init();
        }

        void keeper::(){
          
        }
        void Keeper::updateLine(){
          line_angle = angle.to180(data.dp.line_angle); 
          line_dis   = data.dp.line_distance;
        }
        void Keeper::updateBall(){
          ball_angle = angle.to180(data.dp.ball_angle);
          ball_dis   = data.dp.ball_distance;
        }
        void Keeper::updateCam(){

        }
        void Keeper::updateUART(){
          uart.updateUART();
        }



       Vector Keeper::ballV(){
          Vector v = makeV(reverseAngle(0),0);
          if(ball_angle != 400){
            v = makeV(reverseAngle(ball_angle), percentToPWM(80,0,780));//最大 780 の 80%
            return v;
          }
          return v;
        }

        Vector Keeper::lineV(){
          Vector v = makeV(reverseAngle(0),0);
          float power = line_dis * line_kp;//現在比例のみ
          float angle = line_angle;
          v = makeV(reverseAngle(angle), power);
          return v;
        }

        Vector Keeper::TlineV(Vector v){
          keeperDash_count = 0;
          v.y = 0;
          if(v.x<0)
            v.x = -1.5 * v.x;
          return v;
        }


        Vector Keeper::removeV(){ 
            Vector v = makeV(reverseAngle(0),0);
            float line_angle360 = angle.to360(line_angle);
            float ball_angle360 = angle.to360(ball_angle);
            float line_left = angle.normalization360(line_angle360 - 90);
            float line_right = angle.normalization360(line_angle360 + 90);
            if(line_angle == 400){
              return v;
            }
            if(ball_angle == 400){
              return v;
            }
            //正面の0度をまたいだとき
            if(line_right < line_left){
                //ボールと自分の間にラインあり
                if(ball_angle360 >= line_left || ball_angle360 <= line_right){
                    remove_angle = angle.normalization360(line_angle360 + 180);
                    ball_angleL = ball_angle360 - line_angle360; 
                }
                else{
                  remove_angle = line_angle360;
                  ball_angleL = ball_angle360 - angle.normalization360(line_angle360 + 180);
                }
                if(abs(ball_angleL) > 180){
                  ball_angleL = (360 - ball_angleL) * ball_angleL/abs(ball_angleL);
                }  
              }
              //正面の0度をまたいでいないとき
              else{
                if(ball_angle360 > line_left && ball_angle360 < line_right){//ボールと自分の間にラインあり
                    remove_angle = angle.normalization360(line_angle360 + 180);
                    ball_angleL = ball_angle360 - line_angle360; 
                }
                else{//ボールが前、線が後ろ
                  remove_angle = line_angle360;
                  ball_angleL = angle.normalization360(ball_angle360 - angle.normalization360(line_angle360 + 180));
                }
                if(abs(ball_angleL) > 180){
                  ball_angleL = (360 - ball_angleL) * -1;
                }
              }
            float absolute_ballV = sqrt(ballV().x * ballV().x + ballV().y * ballV().y);
            remove_power = 1.00 * absolute_ballV * cos(ball_angleL * PI/180);
            v = makeV(reverseAngle(remove_angle),abs(remove_power));
            return v;
        }

        Vector Keeper::notToOwnGoal(Vector v){
          return v;
        }

 
        Vector Keeper::lineTraceV(){//ライン検出時、キーパーの動き
          Vector v = makeV(reverseAngle(0),0);
          //ライン見えなくなったらすぐreturn
          if(line_angle == 400){
              state = lost_line;
              return v;
          }
          //ライントレース
          else{
            v = ballV();
            v = addV(v, lineV());
            v = addV(v, removeV());
            bool is_sliding = (abs(line_angle) < 30 || abs(line_angle) > 140);
            // if(is_sliding){
            //     v = addV(v, cameraBrakeV());
            // }
            v = notToOwnGoal(v);
            //T字
            if(line_angle == 500){
              v = TlineV(v);
              return v;
            }
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
            return v;
          }
        }
    
        //カメラを使って戻る動き
        Vector Keeper::lostGoalCameraV(){
          Vector v = makeV(reverseAngle(0),0);
          //ゴールを検出できなければ超音波に切り替え
          if(c_x == 400){
              state = back_to_goal_withECHO;
              return v;
          }
          //正面とみなして、後ろにまっすぐ戻る
          else if(abs(c_x) < 30){
              v = makeV(reverseAngle(180),150);
              return v;
          }
          else if(c_x > 0){
            move_power_cam = c_x * 100/c_x_max;
            if(c_x_max < c_x){move_power_cam = 100;}
            v.y = percentToPWM(move_power_cam,60,180);
          }
          else{
            move_power_cam = -1 * c_x * 100/c_x_max;
            if(c_x_max < -1 * c_x){move_power_cam = 100;}
            v.y = -1 * percentToPWM(move_power_cam,60,180);    
          }

        return v;
        }

        //ライン未検出時、lastlineに戻る動き
        Vector Keeper::lostLineV(){
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
              return v;
          }
          //横移動でラインアウトしてしまった場合もカメラで戻る(旧機体は必須)
          bool is_over_run = (abs(c_x) > 90 && v.x < 0);//c_xのしきい値要調整
          if(is_over_run){
              state = back_to_goal_withCAM;
              return v;
          }
          lost_count++;
          v = last_line;//lineV()内で更新
          return v;
        }


        Vector Keeper::dashV(){
          Vector v = makeV(reverseAngle(0),0);

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
          return v;
        }

        void Keeper::main(){
          Vector v;
          switch(state){
              case out_of_running:
                v = makeV(reverseAngle(0),0);
                break;
              case calibrating:
                v = makeV(reverseAngle(0),0);
                break;
              case line_tracing:
                v = lineTraceV();
              case lost_line:
                v = lostLineV();
                break;
              case keeper_dash:
                v = dashV();
                break;
              case back_to_goal_withCAM:
                v = lostGoalCameraV();
                break;
              case back_to_goal_withECHO:
                v = echo.lostGoalEchoV();
                break;
          }
          main_v = v;
        }

        void Keeper::debugSerial(){
          
        }


        void Keeper::updateAllData(){
          gyro.updateGyro();
          updateUART();
          echo.update();
          updateLine();
          updateBall();
          updateCam();
        }

