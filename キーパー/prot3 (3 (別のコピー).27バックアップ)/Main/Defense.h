    #pragma once
    #include <Arduino.h>
    #include "Instance.h"
    #include <Adafruit_NeoPixel.h>
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

        int16_t N_raw;
        int16_t NE_raw;
        int16_t E_raw;
        int16_t SE_raw;
        int16_t S_raw;
        int16_t SW_raw;
        int16_t W_raw;
        int16_t NW_raw;

        //脱出
        //●脱出速度
        float leave_kp = 4.0f;

        float echo_dif = 500.0f;
        float central_power = 0;
        float central_angle;
        float righter_power = 0;
        float righter_angle;
        float lefter_power = 0;
        float lefter_angle;
        float back_wall_power;
        Vector central = {0,0};
        Vector righter = {0,0};
        Vector lefter  = {0,0};
        Vector back_wall = {0,0};


        //●脱出直前おしりトレース用
                            bool  bottom_trace      = true; //true ⇒ おしりトレース有効化
        static constexpr uint8_t  bottom_trace_dis  = 75; 
        static constexpr float    bottom_trace_kp   = 5.0f;//1.5f
        static constexpr float    lost_bottom_kp    = 5.0f;////lost_lineV用

        //●脱出直前サイドトレース用
        uint8_t side_trace = 1;//
        static constexpr uint8_t  side_trace_dis = 75; 
        static constexpr float    side_trace_kp  = 1.5f;
        static constexpr float    lost_side_kp   = 5.0f;////5.0lost_lineV用

        void convertLine_sideOrBottom();
        void updateLineOrBottom();

        //●超音波復帰用
        static constexpr float lost_goal_echo_kp = 1.2;
        static constexpr int16_t speed_back = 300;

        int16_t ave[8] = {0,0,0,0,0,0,0,0};

        //●超音波移動平均 
        float new_data_ratio = 0.4;

        //●背後からの脱出速度
        float leave_power_S = 300;

        //●壁の距離///////////////////////////
        //後ろ壁ラインアウト対策
        float wall_side = 168.0f;//ラインアウト対策用
        float wall_S = 90.0f;   //ラインアウト対策用
        float goal_area_n = 400.0f;//相手ゴール
        float goal_area_s = 350.0f;//守るべきゴール
        float wall_w = 650.0f;
                float wall_h = 2180.0f;


        //部室
        //壁の距離はキャリブレーションするべし///////////////////////////
        //後ろ壁ラインアウト対策
        // float wall_side = 150.0f;//ラインアウト対策用
        // float wall_S = 65.0f;   //ラインアウト対策用
        // float goal_area_n = 400.0f;//相手ゴール
        // float goal_area_s = 320.0f;//守るべきゴール
        // float wall_w = 593.0f;
        // float wall_h = 1236.0f;

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
        //ラインセンサー故障時に使用
        Vector withoutLineV();
        Vector leaveLineV(char direction);
        //横幅のキャリブレーション。多分使わない
        void calibrateW();
    };



    class Defense{
        private:
            Echo echo;
            Adafruit_NeoPixel* pixels;  // ポインタで受ける

            //キーパー全体で使う
            //●falseでシリアル、LED止める
            static constexpr bool is_use_debug = false;


            static constexpr uint8_t LED = 15;
            static constexpr uint8_t SW = 3;
            uint16_t keeperDash_count = 0;
            Vector last_line;
            Vector defense_v;
            float defense_rotate;
            char leave_mode = 'N';//壁の方向で定義
            float vel_power;
            float vel_x;
            float vel_y;

            //カメラ
            //●trueでカメラ無効化
            bool is_off_cam = false;

            //●<<<<重要>>>>> true ⇒ 青に攻める | false ⇒ 黄色に攻める
            bool is_atack_to_BLUE = true;

            //●ゴールから遠いと判断するしきい値
            static constexpr int16_t cam_far_from_goal = 150;


            //守るゴール
            int16_t cam_angle = 400;
            int16_t cam_dis = -1;
            //攻めるゴール
            int16_t cam_atack_angle = 400;
            int16_t cam_atack_dis   = -1;

            //キーパーダッシュ
            //●falseでキーパーダッシュ無効化
            bool on_dash = false;

            //●近いと判断するball_dis
            int16_t dash_ball_dis = 3;

            //ナナメ前の無効化判断
            //int16_t echo_off_angle = 120;///////仮
            //float   echo_off_dis   = 300;///////仮

            int dash_back_count = 0;
            int dash_count;
            static constexpr int how_much_dash = 400;

            //●キーパーダッシュ連続作動防止
            const uint16_t wait_time = 3000; //3秒

            bool is_enough_time;
            unsigned long start_time;
            unsigned long dash_interval_now;

            //●trueで、相手ゴール脱出を無効化
            static constexpr bool off_front_leave = true;
            //●trueで、サイド脱出の無効化
            static constexpr bool off_side_leave = false;

            //●サイド脱出のカウントしきい値
            static constexpr uint8_t side_leave_timing = 200;

        public:
            //キーパーダッシュ
            bool keeper_dashing = false;
            float fronts_ave;
            //

            //LED
            #define PIN 15
            #define NUMPIXELS 1
            Adafruit_NeoPixel pixel;
            Defense() : pixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800){}
            uint32_t LED_color;

            //ライントレースで使う
            //●lost_lineV kp
            static constexpr float  lost_line_kp = 6.0f;
            static constexpr float line_default_kp = 5.00f;//4.0f
            static constexpr float T_line_minus_power = -1.3;//T字の反発力

                             float line_kp = 6.00f;
            static constexpr float line_ki = 0.00f;
            static constexpr float line_kd = 0.00f;

            //●速度を使うかどうか
            bool is_use_ball_speed = true;

            float line_P;
            float line_I;
            float line_D;
            int   lost_count = 0;
            int leave_count;

            //getBallD用
            static constexpr int16_t BALL_D_INTERVAL = 50;  // ms
            static constexpr float ball_speed_max = 1000.0f;
            static constexpr float ball_speed_kp = 0.60f;
            float ball_d;
            uint32_t last_time;
            int last_ball_angle;
            float last_ball_d = 0.0f;


            float ball_angle;
            float ball_dis;
            float ball_speed;
            float line_angle;
            float line_dis; 
            float angleZ;

            //●最高速度 
            float pwm_max = 780.0f;

            bool is_on_tate = false;
            bool is_on_yoko = false;
            bool is_on_atack_goal = false;
            
            enum State{
                OFF,//0
                line_tracing,//1
                lost_line,//2
                keeper_dash,//3
                back_to_goal_withCAM,//4
                back_to_goal_withECHO,//5
                leave_line,//6
                self_trace,//7
                leave_dash//8
            };
            State state = OFF;

            void init();
            //void setPixels(Adafruit_NeoPixel* p);
            void LED_init();
            void ledOff();
            void debugSerial();
            void debugLED();
            int  pwmDomain(int val);//最大値、最小値の中に留める関数(404で無効化)
            void off();
            void update();
            void setBehavior(State s);
            void finishKeeperDash();

            void updateLine();
            void updateBall();
            void updateCam();
            void updateMPU();
            void updateVelocity();
            void updateAllData();
        
            float  getBallD();
            Vector ballV();
            Vector lineV();
            Vector lineV_onlyP();
            Vector selfTraceV(float perent);
            Vector TlineV(Vector v);
            Vector notToOwnGoal(Vector v);
            Vector cameraV();
            Vector leaveDashV();
            Vector dashV();

            void lostGoalEcho();
            void lineTrace();
            void lostGoalCamera();//カメラを使って戻る動き
            void lostLine();//ライン未検出時、lastlineに戻る動き
            uint8_t isNeedToDash();
            void leaveDash();
            void dash();
            void leaveLine(char direction); //D :キーパーダッシュ
            void selfTrace();
            uint16_t isNeedToLeave();//0:問題なし 1:脱出  2:500 

            uint8_t getH_axis();//0:正面エリア 1:右側エリア 2
            uint8_t isNeedTo_Dash();
            uint8_t isNeedTo_BackFromDash();

    };