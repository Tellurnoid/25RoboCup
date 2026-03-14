#include <Arduino.h>
#include "functions.h"
#include "UART.h"
#include "IMU.h"
#include "sound.h"
#include "RCJ_Japan_Keeper.h"

float c_x;

// class AngleOperation{
//   public:
//     float to360(float theta){
//       if(theta < 0){
//         theta = 360 + theta;
//       }
//       return theta;
//     }
  
//     float to180(float theta){
//        if(theta > 180){
//          theta = theta - 360;
//        }
//        return theta;
//      }

//     float normalization360(float theta){
//       if(theta > 360){
//         theta = theta - 360;
//       }
//       else if(theta < 0){
//         theta = theta + 360;
//       }
//       return theta;
//     }

//     float normalization180(float theta){//角度計算後の正規化
//       if(theta > 180){
//         theta = -1  * (360 - theta);
//       }
//       else if(theta < -180){
//         theta = 360 - abs(theta);
//       }
//       return theta;
//     }
//   private:

// };

class Echo{
  private:

  public:
    //前から時計回り(前=N 右=E 後=W 左=N)
    int16_t N   = data_hub.echo_0;
    int16_t NE  = data_hub.echo_1;
    int16_t E   = data_hub.echo_2;
    int16_t SE  = data_hub.echo_3;
    int16_t S   = data_hub.echo_4;
    int16_t SW  = data_hub.echo_5;
    int16_t W   = data_hub.echo_6;
    int16_t NW  = data_hub.echo_7;
    Vector lostGoalEchoV(){
        Vector v;
        v = makeV(reverseAngle(0),0);
        return v;
    }
};


        // Vector cameraBrakeV(){
        //   Vector v = makeV(reverseAngle(0),0);
        //   float power;
        //   v.x = 0;
        //   if(c_x == 400){
        //     return v;
        //   }
        //   //カーブに入ったら、startとendの距離に対する割合で打ち消し
        //   bool is_sliding = (abs(c_x)>curve_start && abs(c_x)<curve_end);
        //   if(is_sliding){
        //     power =  (abs(c_x) - curve_start) / (float)(curve_end - curve_start);
        //     //左右の区別
        //     if(c_x < curve_start){
        //       power = power * -1;
        //     }
        //     //curve_end超えたら100%
        //     if(abs(c_x)>=curve_end){
        //       power = 1;
        //       if(c_x < curve_start){
        //         power = -1;
        //       }
        //     }
        //   }
        //  v = makeV(reverseAngle(90),PercentToPWM(power,90,255));//角度90度で正しい？要動確///////////////////////
        //  return v;
        // }

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

        void Keeper::init() {
          gyro.initGyro();
        }

        Vector keeper::ballV(){
          Vector v = makeV(reverseAngle(0),0);
          if(ball_angle != 400){
            v = makeV(reverseAngle(ball_angle), PercentToPWM(80,0,780));//最大 780 の 80%
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

        Vector TlineV(Vector v){
          keeperDash_count = 0;
          v.y = 0;
          if(v.x<0)
            v.x = -1.5 * v.x;
          return v;
        }

        Vector lineTraceV(){//ライン検出時、キーパーの動き
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
            if(is_sliding){
                v = addV(v, cameraBrakeV());
            }
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
        Vector lostGoalCameraV(){
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
          Vector v = makeV(reverseAngle(0),0);
          keeperDash_count = 0;
          //復帰した瞬間の回転待ち
          if(abs(angleZ) > 90){
            v = makeV(reverseAngle(0),0);
          }
          //ラインを見失って一定時間経過後、カメラで戻る
          if(lost_count >= 400){
              sound.beepSound();
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


        Vector dashV(){
          Vector v = makeV(reverseAngle(0),0);

          //前に白線がある場合は除外、後ろの超音波見てボール追う
          bool is_line_front = (abs(line_angle) < 45)       ? true : false;
          bool keep_dashing = (is_line_front || line_angle) ? true : false;
          if(keep_dashing && echo.echo_W < 400){
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

        void main(){
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
                v = DashV();
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

        void debugSerial(){
          
        }



Sound sound;
Echo echo;
Keeper keeper;
Gyro gyro;
void setup(){
    initUART();
    sound.init();
    keeper.initMain();
}

void loop(){
    updateUART();
    gyro.updateGyro();
    keeper.main();
}