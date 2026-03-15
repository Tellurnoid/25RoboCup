#pragma once
#include <Arduino.h>
#include "Instance.h"

// class AngleOperation{
//     private:
//     public:
//      float to360(float theta);
//      float to180(float theta);
//      float normalizatoin360(float theta);
//      float normalization180(float theta);
// };

class AngleOperation{
  public:
    //
    float to360(float theta){
      if(theta < 0){
        theta = 360 + theta;
      }
      return theta;
    }
    //convertAngle
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

class Echo{
  private:

  public:
    //前から時計回り(前=N 右=E 後=W 左=N)
    int16_t N;
    int16_t NE;
    int16_t E;
    int16_t SE;
    int16_t S;
    int16_t SW;
    int16_t W;
    int16_t NW;

    void update(){
        N   = data.dp.echo_0;
        NE  = data.dp.echo_1;
        E   = data.dp.echo_2;
        SE  = data.dp.echo_3;
        S   = data.dp.echo_4;
        SW  = data.dp.echo_5;
        W   = data.dp.echo_6;
        NW  = data.dp.echo_7;      
    }

    Vector lostGoalEchoV(){
        Vector v;
        v = makeV(reverseAngle(0),0);
        return v;
    }
};



class Keeper{
    private:
       Echo echo;
       AngleOperation angle;
        //ライントレース
        int line_kp = 6;
        float lineDf_dis;
        float lineDf_angle;
        struct Derivative {
            unsigned long last_time = 0;
            float last_value = 0;
            float value_d = 0;
        };
        Derivative lineD_dis;
        Derivative lineD_angle;
        bool isOnCurve = false;
        float last_angle;

        //打ち消し
        float ball_angleL;//白線の法線から見たボールの角度、360度の方式
        float remove_angle;
        float remove_power;//打ち消しreturn v;

        //キーパーダッシュ
        int half_coat = 80;//170/2
        uint16_t lost_count = 0;

        //キーパー全体で使う
        float ball_angle;
        float ball_dis;
        float line_angle;
        float line_dis; 
        float angleZ;
        uint16_t keeperDash_count = 0;
        Vector last_line;
        Vector main_v;

        //カメラ(line_angle==400時)
        float c_x;
        uint16_t c_x_max = 158;//コートの横端から見たゴール
        uint8_t move_power_cam = 0;
        uint8_t move_angle_cam = 0;
        //キーパーブレーキ用
        const uint16_t curve_start = 50;//要調整//////////////////////////////////////////////////
        const uint16_t curve_end = 90;

        //超音波(line_angle==400時)
        uint8_t coat_w_forPower = 70;//比例定数、切片用
        uint8_t coat_h =  67;
        uint8_t coat_h_forPower =  38;
        uint8_t coat_b = 20;


    public:
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

        void init();
        void debugSerial();

        void updateEcho();
        void updateLine();
        void updateBall();
        void updateCam();
        void updateUART();
        void updateAllData();

        Vector ballV();
        Vector lineV();
        Vector TlineV(Vector v);
        Vector removeV();
        Vector notToOwnGoal(Vector v);
        Vector lineTraceV();

        Vector lostGoalCameraV();//カメラを使って戻る動き
        Vector lostLineV();//ライン未検出時、lastlineに戻る動き

        Vector dashV();

        void main();

};


//コンパイル系 ESP32
// {
//     "port": "/dev/ttyUSB0",
//     "board": "esp32:esp32:esp32",
//     "sketch": "RCJ_Japan_Keeper.ino"
// }


// {
//   "version": "2.0.0",
//   "tasks": [
//     {
//       "label": "compile",
//       "type": "shell",
//       "command": "arduino-cli compile --fqbn esp32:esp32:esp32 --verbose .",
//       "group": "build"
//     },
//     {
//       "label": "upload",
//       "type": "shell",
//       "command": "arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 --verbose .",
//       "dependsOn": "compile"
//     }
//   ]
// }



//廃止済みのコードなど
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