#pragma once
#include <Arduino.h>
#include "UART.h"
#include "IMU.h"

// class AngleOperation{
//     private:
//     public:
//      float to360(float theta);
//      float to180(float theta);
//      float normalizatoin360(float theta);
//      float normalization180(float theta);
// };


class Keeper{
    private:
            //class系
        Echo echo;
        AngleOperation angle;
        Sound sound;
        Gyro gyro;

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

        void updateEcho();
        void updateLine();
        void updateBall();
        void updateCam();
        void updateUART();

        Vector ballV();
        Vector lineV();
        Vector TlineV();
        Vector removeV();
        Vector notToOwnGoal(Vector v);
        Vector lineTraceV();

        Vector lostGoalCameraV();//カメラを使って戻る動き
        Vector lostLineV();//ライン未検出時、lastlineに戻る動き

        Vector dashV();

        void main();

};