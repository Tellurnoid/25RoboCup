#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>   //起動画面(候補２。決まったら一方消してよし)
#include <Fonts/FreeMono9pt7b.h>              //見出しなど
#include <Fonts/FreeSans9pt7b.h>              //値調整UI用
#include "button.h"
#include "commands.h"



class UI{
    private:
        //コンストラクタ
        Button button;
        Command command;

        //絵画系
        static constexpr uint16_t draw_time = 100;
        uint32_t                  last_time = 0;
        

        //UI全体用
        uint8_t cursor_home = 0;
        int     cursor_val_int;
        bool    cursor_val_bool;

        //アプリ
        static constexpr uint8_t NUM_APP = 6;//ここの変更忘れずに
        enum App{
        home,
        logo,
        game,
        led,
        kicker,
        view_vals
        };
        App app = home;

        //view(センサーの値を見るアプリ)の中身
        static constexpr uint8_t NUM_IN_VIEW = 6;//ここの変更忘れずに
        enum App_in_view{
        in_view_select,
        in_view_line,
        in_view_ball,
        in_view_echo,
        in_view_cam,
        in_view_user_vals
        };
        App_in_view app_in_view = in_view_select;

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
        //display display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
        void init();
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
};