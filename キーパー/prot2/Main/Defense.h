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
     uint16_t wall_side = 125;//ラインアウト対策用
     uint16_t wall_S = 125;   //ラインアウト対策用
     uint16_t goal_area_s = 320;
     float wall_w = 700;            
     float wall_h = 618*2;

     //超音波のみで計測した推定位置
     //コート中央を原点とする
     // -wall_w < X < +wall_w
     // -wall_h < Y < +wall_h
     uint16_t tolerance = 100;//許容誤差
     float position_x;
     //uint16_t position_y;
    void update();
    Vector lostGoalEchoV();
    Vector backWallBlockV(Vector v);
    Vector withoutLineV();//ラインセンサー故障時に使用

    //横幅のキャリブレーション。多分使わない
    void calibrateW();
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
        int16_t cam_angle = 400;

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
        int  pwmDomain(int val);//最大値、最小値の中に留める関数(404で無効化)
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
        Vector selfTraceV(float perent);
        Vector TlineV(Vector v);
        Vector notToOwnGoal(Vector v);
        void lostGoalEcho();
        void lineTrace();
        void lostGoalCamera();//カメラを使って戻る動き
        void lostLine();//ライン未検出時、lastlineに戻る動き
        void dash();
};