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


    float kp = 0.8f;
    float echo_dif = 500.0f;
    float central_power;
    float central_angle;
    float righter_power;
    float righter_angle;
    float lefter_power;
    float lefter_angle;
    float back_wall_power;
    Vector central = {0,0};
    Vector righter = {0,0};
    Vector lefter  = {0,0};
    Vector back_wall = {0,0};




    int16_t ave[8] = {0,0,0,0,0,0,0,0};
    float new_data_ratio = 0.4;
    //壁の距離はキャリブレーションするべし///////////////////////////
    //後ろ壁ラインアウト対策
     float wall_side = 170.0f;//ラインアウト対策用
     float wall_S = 125.0f;   //ラインアウト対策用
     float goal_area_s = 320.0f;
     float wall_w = 700.0f;            
     float wall_h = 1236.0f;

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
    Vector leaveLineV(char direction);
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
        char leave_mode = 'N';//壁の方向で定義

        //ライントレースで使う
        float line_kp = 6.00f;
        float line_ki = 3.00f;
        float line_kd = 0.50f;
        float line_P;
        float line_I;
        float line_D;
        int   lost_count = 0;
        bool is_on_tate = false;
        bool is_on_yoko = false;

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
            out_of_running,//0
            line_tracing,//1
            lost_line,//2
            keeper_dash,//3
            back_to_goal_withCAM,//4
            back_to_goal_withECHO,//5
            leave_line,//6
            self_trace//7
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
        void leaveLine(char direction);
        void selfTrace();
};