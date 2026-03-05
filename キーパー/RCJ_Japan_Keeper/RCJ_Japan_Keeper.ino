#include <Arduino.h>
#include "UART.h"
#include "IMU.h"
#include "functions.h"

class Sound{//IMU.inoの出力は独立しているため注意
  public:
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
  private:

};

class AngleOperation{
  public:
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
  private:

};

struct Echo{
  public:
    Vector lostGoalEchoV(){
        Vector v;
        v = makeV(0,0);
        return v;
    }
  private:
};

class Keeper{
  private:
        //EchoとSoundに依存
      Echo echo;
      AngleOperation angle;
      Sound sound;
      //ライントレース
      float lineDf_dis;
      float lineDf_angle;
      Derivative lineD_dis;
      Derivative lineD_angle;
      bool isOnCurve = false;
      float last_angle;

      //打ち消し
      float ball_angleL;//白線の法線から見たボールの角度、360度の方式
      float remove_angle;
      float remove_power;//打ち消し

      //キーパーダッシュ
      int half_coat = 80;//170/2
      uint16_t lost_count = 0;

      //キーパー全体で使う
      uint16_t keeperDash_count = 0;
      Vector last_line;

      //カメラ(line_angle==400時)
      uint16_t c_x_max = 158;//コートの横端から見たゴール
      uint8_t move_power_cam = 0;
      uint8_t move_angle_cam = 0;
      //キーパーブレーキ用
      const uint16_t curve_start = 50;//要調整//////////////////////////////////////////////////
      const uint16_t curve_end = 90;

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
          float power;
          v.x = 0;
          if(c_x == 400){
            return;
          }
          //カーブに入ったら、startとendの距離に対する割合で打ち消し
          bool is_sliding = (abs(c_x)>curve_start && abs(c_x)<curve_end);
          if(is_sliding){
            power =  (abs(c_x) - curve_start) / (float)(curve_end - curve_start);
            //左右の区別
            if(c_x < curve_start){
              power = power * -1;
            }
            //curve_end超えたら100%
            if(abs(c_x)>=curve_end){
              power = 1;
              if(c_x < curve_start){
                power = -1;
              }
            }
          }
         v = makeV(90,PercentToPWM(power,90,255));//角度90度で正しい？要動確///////////////////////
         return v;
        }

        Vector removeV(){ 
            float line_angle360 = angle.to360(line_angle);
            float ball_angle360 = angle.to360(ball_angle);
            float line_left = angle.normalization360(line_angle360 - 90);
            float line_right = angle.normalization360(line_angle360 + 90);
            if(line_angle == 400){
              return;
            }
            if(ball_angle == 400){
              return;
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
            return makeV(remove_angle,abs(remove_power));
        }

        Vector notToOwnGoal(Vector v){

        }

  public:
      //超音波(line_angle==400時)
      uint8_t coat_w_forPower = 70;//比例定数、切片用
      uint8_t coat_h =  67;
      uint8_t coat_h_forPower =  38;
      uint8_t coat_b = 20;

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