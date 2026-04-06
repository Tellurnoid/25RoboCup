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
            int16_t ave[8] = {0,0,0,0,0,0,0,0};
            float new_data_ratio = 0.4;

            //●壁の距離
            float wall_side   = 150.0f;//ラインアウト対策用
            float wall_S      = 125.0f;   //ラインアウト対策用
            float goal_area_n = 400.0f;//相手ゴール
            float goal_area_s = 320.0f;//守るべきゴール
            float coat_w = 593.0f;
            float coat_h = 618.0f;
            //T字入る前のセーフティネット
            static constexpr float before_T_s = 130.0f;

            void update();
            Vector backWallBlockV(Vector v);
    };

    class Defense{
        private:
            Echo echo;
            Adafruit_NeoPixel* pixels; 

            Vector last_line;
            Vector defense_v;
            float defense_rotate;
            char leave_mode = 'N';//壁の方向
            float ball_angle;
            float ball_dis;
            float ball_speed;
            float line_angle;
            float line_dis; 
            int   lost_count = 0;
            float angleZ;
            int16_t cam_angle = 400;
            int16_t cam_dis = -1;
            int16_t cam_atack_angle = 400;
            int16_t cam_atack_dis   = -1;
            float pwm_max = 780.0f;
            bool is_on_tate = false;
            bool is_on_yoko = false;
            bool is_on_atack_goal = false;
            //getBallD用
            static constexpr int16_t BALL_D_INTERVAL = 50;  // ms
            static constexpr float ball_speed_max = 1000.0f;
            static constexpr float ball_speed_kp = 0.60f;
            float ball_d;
            uint32_t last_time;
            int last_ball_angle;
            float last_ball_d = 0.0f;

            //●デバッグ
            //falseでLED,シリアルモニタ無効化
            static constexpr bool use_debug = false;
           
            //●ライントレース
            static constexpr float line_kp    = 5.0f;
            static constexpr float lost_kp    = 6.0f;
            static constexpr float no_kp_dis  = 10.0f; 
            //T字の反発力
            static constexpr float T_kp    = 1.3f;
            //ballVの速度
            float ballV_speed = pwm_max;
            //ボール速度を使用
            static constexpr bool  use_ball_speed = true;
            static constexpr int   lost_count_max = 400;

            //●セルフトレース
            //falseで無効化
            static constexpr bool self_trace_on = false;
            //セルフトレースの適用基準
            static constexpr int16_t self_on_angle = 120;
            static constexpr int16_t self_trace_speed = 300;

            //●キーパーダッシュ
            //trueでキーパーダッシュ有効化
            static constexpr bool on_dash = true;
            //ball_disの基準(n以上)
            static constexpr int16_t dash_ball_dis = 5; 
            //キーパーダッシュ連続作動防止
            unsigned long dash_start_time;
            const uint16_t dash_wait_time = 3000; // 3秒

            //●カメラ
            //false ⇒ カメラ無効化
            static constexpr bool cam_on = true; 
            //ゴールから遠いと判断するしきい値
            static constexpr int16_t cam_far_from_goal = 120;
            //回り込み用角度係数
            static constexpr float cameraV_angle_kp = 1.2;
            static constexpr float cameraV_speed    = 500;
            //●超音波復帰
            static constexpr float lost_echo_speed = 300;
        public:

            //LED
            #define LEDPIN 15
            #define NUMPIXELS 1
            Adafruit_NeoPixel pixel;
            Defense() : pixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800){}
            uint32_t LED_color;

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

            void LED_init();
            void init();
            void setBehavior(State s);
            void ledOff();
            void off();
            void debugSerial();
            void debugLED();

            void update();
            void updateMPU();
            void updateLine();
            void updateBall();
            void updateCam();
            void updateAllData();

            float  getBallD();
            Vector ballV();
            Vector lineV();
            Vector selfTraceV();
            Vector TlineV(Vector v);
            Vector notToOwnGoal();
            Vector cameraV();

            void lineTrace();
            void selfTrace();
            void lostLine();
            Vector lostGoalEchoV();
            void lostGoalEcho();
            void lostGoalCamera();

            void finishKeeperDash();

            uint8_t isNeedTo_dash();
            uint8_t isNeedTo_leave();
    };