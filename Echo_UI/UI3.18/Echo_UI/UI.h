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
        
        //game用
        bool is_on_game = false;
        static constexpr uint16_t NUM_MODE = 2;

        //値変更関数用
        int count = 0;

        struct States{
            uint8_t num;
            const char* name;
            void (UI::*func)();
        };
        uint8_t app_state = 0;
        

    public:
        static constexpr uint8_t  SCREEN_WIDTH  = 128;
        static constexpr uint8_t  SCREEN_HEIGHT = 64;  // 座標は0~63
        static constexpr int8_t   OLED_RESET    = -1;
        static constexpr uint8_t  SCREEN_ADDRESS = 0x3C;
        Adafruit_SSD1306 display;
        UI() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}
        void init();
        void startUp();
        void main();
        void app_home();
        void app_game();
        void app_kicker();
        void app_line();
        void app_ball();
        void app_gyro();
        void app_echo();
        void app_cam();
        void app_logo();
        void writeNumber(int x, int y, int number);
        void changeIntVal(const char* name,int &val,int min,int max,int default_val);
        void changeBoolVal(const char* name,bool &val,bool default_val);

        static States app[];         
        static const size_t NUM_APP;

        //Camera用
        uint8_t ang_blue = 0;
        uint8_t ang_yellow = 0;
        uint8_t cd_blue = 0;
        uint8_t cd_yellow = 0;
        uint8_t which_cam = 0;//0:青が前,黄色が後ろ   1:黄色が前,青が後ろ

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
            display.setFont();
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

        private:
        //128x16
          static constexpr uint8_t URAWA_BEOLSAE_logo[1024] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x10, 0x09, 0xf8, 0x11, 0x01, 0x01, 0x08, 0x03, 0xf0, 0xfe, 0x1f, 0x04, 0x03, 0xc0, 0x81, 0xfc,
            0x10, 0x09, 0x0c, 0x39, 0x81, 0x03, 0x08, 0x02, 0x08, 0x80, 0x20, 0x84, 0x04, 0x20, 0x81, 0x00,
            0x10, 0x09, 0x04, 0x28, 0x82, 0x82, 0x14, 0x02, 0x08, 0x80, 0x40, 0x44, 0x08, 0x01, 0x41, 0x00,
            0x10, 0x09, 0x04, 0x2c, 0x82, 0x82, 0x14, 0x02, 0x08, 0x80, 0x80, 0x24, 0x08, 0x03, 0x41, 0x00,
            0x10, 0x09, 0x04, 0x64, 0xc2, 0x82, 0x22, 0x02, 0x08, 0x80, 0x80, 0x24, 0x08, 0x02, 0x21, 0x00,
            0x10, 0x09, 0x08, 0x44, 0x44, 0x44, 0x22, 0x02, 0x10, 0x80, 0x80, 0x24, 0x06, 0x02, 0x21, 0x00,
            0x10, 0x09, 0xf0, 0xc6, 0x44, 0x44, 0x62, 0x03, 0xf0, 0xfc, 0x80, 0x24, 0x01, 0x86, 0x21, 0xf8,
            0x10, 0x09, 0x20, 0x82, 0x44, 0x44, 0x41, 0x02, 0x08, 0x80, 0x80, 0x24, 0x00, 0x44, 0x11, 0x00,
            0x10, 0x09, 0x10, 0xfe, 0x28, 0x28, 0x7f, 0x02, 0x04, 0x80, 0x80, 0x24, 0x00, 0x27, 0xf1, 0x00,
            0x18, 0x19, 0x19, 0x03, 0x28, 0x28, 0x80, 0x82, 0x04, 0x80, 0x80, 0x24, 0x00, 0x28, 0x09, 0x00,
            0x08, 0x11, 0x09, 0x01, 0x28, 0x28, 0x80, 0x82, 0x04, 0x80, 0x40, 0x44, 0x00, 0x28, 0x09, 0x00,
            0x0c, 0x31, 0x05, 0x01, 0x10, 0x10, 0x80, 0x82, 0x08, 0x80, 0x20, 0x84, 0x08, 0x48, 0x09, 0x00,
            0x07, 0xe1, 0x02, 0x00, 0x90, 0x11, 0x00, 0x43, 0xf0, 0xfe, 0x1f, 0x07, 0xe7, 0x90, 0x05, 0xfc,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
 
};