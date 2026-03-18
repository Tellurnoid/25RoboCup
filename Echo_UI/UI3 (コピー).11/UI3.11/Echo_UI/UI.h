#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include "Instance.h"
#include "button.h"
#include "sound.h"
#include "echo.h"
//#include "commands.h"

/*
FreeMono12pt7b.h		FreeSansBoldOblique12pt7b.h
FreeMono18pt7b.h		FreeSansBoldOblique18pt7b.h
FreeMono24pt7b.h		FreeSansBoldOblique24pt7b.h
FreeMono9pt7b.h			FreeSansBoldOblique9pt7b.h
FreeMonoBold12pt7b.h		FreeSansOblique12pt7b.h
FreeMonoBold18pt7b.h		FreeSansOblique18pt7b.h
FreeMonoBold24pt7b.h		FreeSansOblique24pt7b.h
FreeMonoBold9pt7b.h		FreeSansOblique9pt7b.h
FreeMonoBoldOblique12pt7b.h	FreeSerif12pt7b.h
FreeMonoBoldOblique18pt7b.h	FreeSerif18pt7b.h
FreeMonoBoldOblique24pt7b.h	FreeSerif24pt7b.h
FreeMonoBoldOblique9pt7b.h	FreeSerif9pt7b.h
FreeMonoOblique12pt7b.h		FreeSerifBold12pt7b.h
FreeMonoOblique18pt7b.h		FreeSerifBold18pt7b.h
FreeMonoOblique24pt7b.h		FreeSerifBold24pt7b.h
FreeMonoOblique9pt7b.h		FreeSerifBold9pt7b.h
FreeSans12pt7b.h		FreeSerifBoldItalic12pt7b.h
FreeSans18pt7b.h		FreeSerifBoldItalic18pt7b.h
FreeSans24pt7b.h		FreeSerifBoldItalic24pt7b.h
FreeSans9pt7b.h			FreeSerifBoldItalic9pt7b.h
FreeSansBold12pt7b.h		FreeSerifItalic12pt7b.h
FreeSansBold18pt7b.h		FreeSerifItalic18pt7b.h
FreeSansBold24pt7b.h		FreeSerifItalic24pt7b.h
FreeSansBold9pt7b.h		FreeSerifItalic9pt7b.h
*/


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
        static constexpr uint8_t NUM_APP = 7;//ここの変更忘れずに
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
            {3,"LineCalibrate"},
            {4,"led"},
            {5,"kicker"},
            {6,"view_vals"}
        };
        uint8_t app_state = 0;

        //view(センサーの値を見るアプリ)の中身
        static constexpr uint8_t NUM_IN_VIEW = 7;//ここの変更忘れずに
        States App_in_view[NUM_IN_VIEW] = {
            {0,"Select"},
            {1,"Line"},
            {2,"Ball"},
            {3,"Gyro"},
            {4,"Echo"},
            {5,"Cam"},
            {6,"User's"}
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
        void app_line_calibrate();
        void app_led();
        void app_kicker();
        void app_view_vals();
        void app_in_view_select();
        void app_in_view_line();
        void app_in_view_ball();
        void app_in_view_gyro();
        void app_in_view_echo();
        void app_in_view_cam();
        void app_in_view_user_vals();//シリアルモニター
        void app_logo();
        void changeIntVal(const char* name,int &val,int min,int max,int default_val);
        void changeBoolVal(const char* name,bool &val,bool default_val);

        template <class T>
        void simpleSwitch(
                            uint8_t triger_push,
                            T* obj,
                            const char* nameA, void (T::*funcA)(),
                            const char* nameB, void (T::*funcB)(),
                            const char* nameC, void (T::*funcC)()
                        )
            {
            back  = button.read(button.BACK);
            enter = button.read(button.ENTER);
            left  = button.read(button.LEFT);
            right = button.read(button.RIGHT);
            const uint8_t height_enter = 5;
            const uint8_t radius_enter = 15;
            const uint8_t height_mini = 2;
            const uint8_t radius_mini = 7;
            display.clearDisplay();
            //ボタンのアニメーション
            //押されていないときの絵画
            if(enter==0){
                //大きいやつ
                for(int i=height_enter; i>=0; i--){
                    display.fillCircle(80,22+i,radius_enter,1);
                }
                display.fillCircle(80,22,radius_enter-1,0);
                display.drawCircle(80,22,radius_enter,1);
            }
            if(left==0){
                //小さいやつ(上)
                for(int i=height_mini; i>=0; i--){
                    display.fillCircle(5+radius_mini,16+i-radius_mini,radius_mini,1);
                }
                display.fillCircle(5+radius_mini,16-radius_mini,radius_mini-1,0);
                display.drawCircle(5+radius_mini,16-radius_mini,radius_mini,1);
            }
            if(right==0){
                //小さいやつ(下)
                for(int i=height_mini; i>=0; i--){
                    display.fillCircle(5+radius_mini,38+i-radius_mini,radius_mini,1);
                }
                display.fillCircle(5+radius_mini,38-radius_mini,radius_mini-1,0);
                display.drawCircle(5+radius_mini,38-radius_mini,radius_mini,1);
            }
            display.setTextSize(1);
            display.setTextColor(1);
            display.setCursor(85,0);
            display.print(nameA);
            display.setCursor(radius_mini*4,29);
            display.print(nameB);
            display.setCursor(radius_mini*4,13);
            display.print(nameC);
            display.setCursor(0,53);
            display.print("enter to execute");

            if(enter==triger_push){
              (obj->*funcA)();//引数の関数を実行
            }
            if(left==triger_push){
              (obj->*funcB)();//引数の関数を実行
            }
            if(right==triger_push){
              (obj->*funcC)();//引数の関数を実行
            }
            if(enter!=0){
              display.drawCircle(80,22+height_enter,radius_enter,1);
            }
            if(left!=0){
                display.drawCircle(5+radius_mini,16+height_mini-radius_mini,radius_mini,1);
            }
            if(right!=0){
                display.drawCircle(5+radius_mini,38+height_mini-radius_mini,radius_mini,1);
            }
        }
};