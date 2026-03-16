#pragma once
#include <Arduino.h>
#include "Instance.h"


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

    //壁の距離はキャリブレーションするべし///////////////////////////
    //後ろ壁ラインアウト対策
     
     uint16_t wall_S = 125;
     uint16_t goal_area_s = 325;
     uint16_t wall_w = 695*2;
     uint16_t wall_h = 618*2;
    void update();
    Vector lostGoalEchoV();
    Vector backWallBlockV(Vector v);
};



class Defense{
    private:
        Echo echo;
        //キーパー全体で使う
        uint16_t keeperDash_count = 0;
        Vector last_line;
        Vector defense_v;
        float defense_rotate;

        //ライントレースで使う
        float line_kp = 6.00f;
        float line_ki = 3.00f;
        float line_kd = 0.50f;
        float line_P;
        float line_I;
        float line_D;
        int   lost_count = 0;

        //カメラ
        int16_t c_x = 400;

    public:
        float ball_angle;
        float ball_dis;
        float line_angle;
        float line_dis; 
        float angleZ;
        float pwm_max = 780.0f;
    
        enum State{
            out_of_running,
            line_tracing,
            lost_line,
            keeper_dash,
            back_to_goal_withCAM,
            back_to_goal_withECHO,
        };
        State state = out_of_running;

        void init();
        void debugSerial();
        void off();
        void update();
        void setBehavior(State s);

        void updateLine();
        void updateBall();
        void updateCam();
        void updateMPU();
        void updateAllData();

        Vector ballV();
        Vector lineV();
        Vector lineV_onlyP();
        Vector TlineV(Vector v);
        Vector notToOwnGoal(Vector v);
        void lostGoalEcho();
        void lineTrace();
        void lostGoalCamera();//カメラを使って戻る動き
        void lostLine();//ライン未検出時、lastlineに戻る動き
        void dash();
};