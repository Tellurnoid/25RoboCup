#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>   //起動画面(候補２。決まったら一方消してよし)
#include <Fonts/FreeMono9pt7b.h>              //見出しなど
#include <Fonts/FreeSans9pt7b.h>              //値調整UI用
#include "button.h"
#include "sound.h"
#include "echo.h"
//#include "commands.h"



class UI{
    private:
        //コンストラクタ
        Button button;
        Echo echo;
        Sound sound;
        //Command command;
        bool pushing = false;
        int8_t back  = button.read(button.BACK);
        int8_t enter = button.read(button.ENTER);
        int8_t left  = button.read(button.LEFT);
        int8_t right = button.read(button.RIGHT);
        //絵画系
        static constexpr uint16_t draw_time = 50;
        uint32_t                  last_time = 0;
        

        //UI全体用
        uint8_t cursor_home = 2;
        int     cursor_val_int;
        bool    cursor_val_bool;

        //home用
        //logo用
        
        //game用
        bool is_on_game = false;
        static constexpr uint16_t NUM_MODE = 2;


        //値変更関数用
        int count = 0;

        //アプリ
        static constexpr uint8_t NUM_APP = 6;//ここの変更忘れずに
        // enum App{
        // home,
        // logo,
        // game,
        // led,
        // kicker,
        // view_vals
        // };
        // App app = home;
        struct States{
            uint8_t num;
            const char* name;
        };
        States app[NUM_APP] = {
            {0,"home"},
            {1,"logo"},
            {2,"game"},
            {3,"led"},
            {4,"kicker"},
            {5,"view_vals"}
        };
        uint8_t app_state = 0;

        //view(センサーの値を見るアプリ)の中身
        static constexpr uint8_t NUM_IN_VIEW = 6;//ここの変更忘れずに
        States App_in_view[NUM_IN_VIEW]{
            {0,"in_view_select"},
            {1,"in_view_line"},
            {2,"in_view_ball"},
            {3,"in_view_echo"},
            {4,"in_view_cam"},
            {5,"in_view_user_vals"}
        };
        uint8_t in_view_state = 0;

    public:

        // static constexpr uint8_t  SCREEN_WIDTH  = 128;
        // static constexpr uint8_t  SCREEN_HEIGHT = 64;  // 座標は0~63
        // static constexpr int8_t   OLED_RESET    = -1;
        // static constexpr uint8_t  SCREEN_ADDRESS = 0x3C;
        static constexpr uint8_t  SCREEN_WIDTH  = 128;
        static constexpr uint8_t  SCREEN_HEIGHT = 64;  // 座標は0~63
        static constexpr int8_t   OLED_RESET    = -1;
        static constexpr uint8_t  SCREEN_ADDRESS = 0x3C;
        Adafruit_SSD1306 display;
        UI() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}
        //display display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
        void init();
        void startUp();
        void writeNumber(int x, int y, int number);
        void main();
        void app_home();
        void app_game();
        void app_led();
        void app_kicker();
        void app_view_vals();
        void app_in_view_select();
        void app_in_view_line();
        void app_in_view_ball();
        void app_in_view_echo();
        void app_in_view_cam();
        void app_in_view_user_vals();
        void app_logo();
        void changeIntVal(const char* name,int &val,int min,int max,int default_val);
        void changeBoolVal(const char* name,bool &val,bool default_val);
};