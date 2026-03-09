#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>   //起動画面(候補２。決まったら一方消してよし)
#include <Fonts/FreeMono9pt7b.h>              //見出しなど
#include <Fonts/FreeSans9pt7b.h>              //値調整UI用
#include "UI.h"
#include "button.h"
#include "commands.h"

    void UI::init() {
      Wire.setSDA(4);
      Wire.setSCL(5);
      Wire.begin();
      Wire.setClock(400000);   // 400kHz

      if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        while (true);
      }

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
    }

    //デバッグ用
    void UI::writeNumber(int x, int y, int number) {
      display.setCursor(x, y);
      display.print(number);
      // display.display();
    }

    void UI::main(){
      switch(app){
        case home:
         app_home();
         break;
        case game:
         app_game();
         break;
        case led:
         app_led();
         break;
        case kicker:
         app_kicker();
         break;
        case view_vals:
         app_view_vals();
         break;
        case logo:
         app_logo();
         break;
      }

      //絵画
      if ((millis() - last_time) >= draw_time) {
        last_time = millis();
        display.display();
      }
    }

    void UI::app_home(){
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextColor(1);
      for (int i=0; i<4; i++){
        display.println(button.buttonStates[i]);
      }
    }
   
    void UI::app_game(){}
    void UI::app_led(){}
    void UI::app_kicker(){}
    void UI::app_logo(){}

    void UI::app_view_vals(){
      switch(app_in_view){
        case in_view_select:
         app_in_view_select();
         break;
        case in_view_line:
         app_in_view_line();
         break;
        case in_view_ball:
         app_in_view_ball();
         break;
        case in_view_echo:
         app_in_view_echo();
         break;
        case in_view_cam:
         app_in_view_cam();
         break;
        case in_view_user_vals:
         app_in_view_user_vals();
         break;
      }
    }
        //view_vals()関数の選択画面
        void UI::app_in_view_select(){
  
        }
        void UI::app_in_view_line(){

        }
        void UI::app_in_view_ball(){

        }
        void UI::app_in_view_echo(){

        }
        void UI::app_in_view_cam(){

        }
        void UI::app_in_view_user_vals(){

        }

