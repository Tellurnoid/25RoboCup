#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

#include "Instance.h"

#include "UI.h"
#include "button.h"
#include "sound.h"
#include "UART.h"
//#include "commands.h"

    void UI::init() {
      Wire.setSDA(4);
      Wire.setSCL(5);
      Wire.begin();
      Wire.setClock(400000);   // 400kHz
      if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        while (true);
      }

    }

    void UI::startUp(){
      display.clearDisplay();
      display.setFont(&FreeSans12pt7b);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,2);
      display.print("BEOLSAE");
      display.display();
    }

    //デバッグ用
    void UI::writeNumber(int x, int y, int number) {
      display.setCursor(x, y);
      display.print(number);
      // display.display();
    }

    void UI::main(){
 

      back  = button.read(button.BACK);
      enter = button.read(button.ENTER);
      left  = button.read(button.LEFT);
      right = button.read(button.RIGHT);
      switch(app[app_state].num){
        case 0:
         app_home();
         break;
        case 1:
         app_game();
         break;
        case 2:
         app_in_view_gyro();
         break;
        case 3:
         app_lineCal();
         break;
        case 4:
         app_led();
         break;
        case 5:
         app_kicker();
         break;

      }
      if(left==1 || right==1)
        sound.cursor();
      if(enter==1)
        sound.cursor();
      if(back==1)
        sound.back();
        
      display.setFont();// 
      display.setTextSize(1);
      // display.setTextColor(SSD1306_WHITE);
      // display.setCursor(0,0);
      // display.print(app[app_state].name);
      //絵画
      if ((millis() - last_time) >= draw_time) {
        last_time = millis();
        display.display();
      }
    }

    void UI::app_home(){
      display.clearDisplay();
      display.setCursor(15,25);
      display.setFont(&FreeSans12pt7b);
      display.setTextColor(1);
      display.print(app[cursor_home].name);
      //ボタンの状態表示
      if(left != 0){
        display.fillRect(0,0,2,64,1);        
      }    
      if(right != 0){
        display.fillRect(126,0,128,64,1);
      }

      //決定
      if(enter==1){
        if(cursor_home == 0){
          //表示専用カーソルのためエラー音
          sound.error();
        }
        else{
          app_state = (cursor_home + NUM_APP) % NUM_APP;
          sound.enter();
          // app_state = cursor_home - 1;
          // if(app_state<0){
          //   app_state = NUM_APP;
          // }
        }
      }
      //カーソル移動
      //左
      if(left==1)
        cursor_home = ((cursor_home + NUM_APP - 1) % NUM_APP);

      //右
      if(right==1)
        cursor_home = ((cursor_home + 1) % NUM_APP);
      
      //戻るキーは無効なのでエラー音など
      if(back!=0){}

      
    }
    void app_lineCal(){
      
    }
   
    void UI::app_game(){
      display.clearDisplay();
      display.setCursor(18,28);
      display.setFont(&FreeSans9pt7b);
      display.setTextColor(1);
      if(cursor_val_int == 0){
        display.print("ATACK");
      }
      else if(cursor_val_int == 1){
        display.print("DIFENCE");
      }
      if(enter!=2 && is_on_game ==true){
        display.fillRect(0,0,128,64,1);
      }

      //カーソル移動
      //左
      if(left==1){
        cursor_val_int = ((cursor_val_int + NUM_MODE - 1) % NUM_MODE);
        if (is_on_game) command.sendCommand(MAIN, START, 0);
        is_on_game = false;
        }
      //右
      if(right==1){
        cursor_val_int = ((cursor_val_int + 1) % NUM_MODE);
        if (is_on_game) command.sendCommand(MAIN, START, 0);
        is_on_game = false;
        }
      if(back!=0){
        app_state = 0;
        if (is_on_game) command.sendCommand(MAIN, START, 0);
        is_on_game = false;
        display.fillRect(0,62,128,64,1);
      }
      if(enter==1){
        is_on_game = !is_on_game;
        //ここに試合開始コマンド
        if (is_on_game && cursor_val_int == 0) command.sendCommand(MAIN, START, 1);
        else if (is_on_game  && cursor_val_int == 1) command.sendCommand(MAIN, START, 2);
        else command.sendCommand(MAIN, START, 0);
        sound.start();
      }
    }
    void UI::app_led(){
      int LEDPWM = 0;
      changeIntVal("LED",LEDPWM,0,255,100);
    }
    void UI::app_kicker(){
      //(トリガー(コマンドなら1,)
      simpleSwitch(2,&sound ,"kicker",&Sound::cursor,"charge",&Sound::enter,"charge",&Sound::error);
            //ホームに戻る
            if(back!=0){
                app_state = 0;
                sound.back();
            }
    }
    void UI::app_logo(){
      display.clearDisplay();
      display.setCursor(15,20);
      display.setFont(&FreeSans12pt7b);
      display.setTextColor(1);
      display.print("LOGO");
      if(back!=0){
        app_state = 0;
        display.fillRect(0,62,128,64,1);
      }
    }

    void UI::app_view_vals(){
      switch(App_in_view[in_view_state].num){
        case 0:
         app_in_view_select();
         break;
        case 1:
         app_in_view_line();
         break;
        case 2:
         app_in_view_ball();
         break;
        case 3:
         app_in_view_gyro();
         break;
        case 4:
         app_in_view_echo();
         break;
        case 5:
         app_in_view_cam();
         break;
        case 6:
         app_in_view_user_vals();
         break;
      }
      if(back!=0){
        app_state = 0;
        display.fillRect(0,62,128,64,1);
      }
    }
        //view_vals()関数の選択画面
        void UI::app_in_view_select(){
          display.clearDisplay();
          display.setCursor(15,20);
          display.setFont(&FreeSans12pt7b);
          display.setTextColor(1);
          display.print(App_in_view[cursor_home].name);
          //ボタンの状態表示
          if(left != 0){
            display.fillRect(0,0,2,64,1);        
          }    
          if(right != 0){
            display.fillRect(126,0,128,64,1);
          }

          //決定
          if(enter==1){
            if(cursor_home == 0){
              //表示専用カーソルのためエラー音
            }
            else{
              in_view_state = (cursor_home + NUM_IN_VIEW) % NUM_IN_VIEW;
              // app_state = cursor_home - 1;
              // if(app_state<0){
              //   app_state = NUM_APP;
              // }
            }
          }
          //カーソル移動
          //左
          if(left==1)
            cursor_home = ((cursor_home + NUM_APP - 1) % NUM_APP);
          //右
          if(right==1)
            cursor_home = ((cursor_home + 1) % NUM_APP);
        }
        void UI::app_in_view_line(){
          display.clearDisplay();
          display.setCursor(15,20);
          display.setFont(&FreeSans12pt7b);
          display.setTextColor(1);
          display.print(App_in_view[in_view_state].name);

          display.print(data.dp.line_angle);
          if(back!=0){
            in_view_state = 0;
            display.fillRect(0,62,128,64,1);
          }
        }
        void UI::app_in_view_ball(){
          display.clearDisplay();
          display.setCursor(15,20);
          display.setFont(&FreeSans12pt7b);
          display.setTextColor(1);
          display.print(App_in_view[in_view_state].name);
          
          display.print(data.dp.ball_angle);
          if(back!=0){
            in_view_state = 0;
            display.fillRect(0,62,128,64,1);
          }
        }
        void UI::app_in_view_gyro(){
          display.clearDisplay();
          display.setCursor(15,20);
          display.setFont(&FreeSans12pt7b);
          display.setTextColor(1);
          display.print(App_in_view[in_view_state].name);
          
          display.print(data.dp.robot_angle);

          if(enter==1){
            command.sendCommand(MAIN, RESET_GYRO, 1);
            sound.start();
          }

          if(back!=0){
            in_view_state = 0;
            display.fillRect(0,62,128,64,1);
          }
        }
        void UI::app_in_view_echo(){
          display.clearDisplay();
          display.setCursor(15,20);
          display.setFont(&FreeSans12pt7b);
          display.setTextColor(1);
          display.print(App_in_view[in_view_state].name);
          if(back!=0){
            in_view_state = 0;
            display.fillRect(0,62,128,64,1);
          }
        }
        void UI::app_in_view_cam(){
          display.clearDisplay();
          display.setCursor(15,20);
          display.setFont(&FreeSans12pt7b);
          display.setTextColor(1);
          display.print(App_in_view[in_view_state].name);
          if(back!=0){
            in_view_state = 0;
            display.fillRect(0,62,128,64,1);
          }
        }
        void UI::app_in_view_user_vals(){
          display.clearDisplay();
          display.setCursor(15,20);
          display.setFont(&FreeSans12pt7b);
          display.setTextColor(1);
          display.print(App_in_view[in_view_state].name);
          if(back!=0){
            in_view_state = 0;
            display.fillRect(0,62,128,64,1);
          }
       }

        //任意のint変数を変更するUI(参照渡し)
        void UI::changeIntVal(const char* name,int &val ,int min,int max,int default_val){
            bool is_changed = false;
            display.clearDisplay();
            display.setCursor(0,0);
            display.setFont();
            display.setTextSize(1);
            display.setTextColor(1);
            display.print("change value:[ ");
            display.print(name);
            display.println(" ]");
            //display.print(count);
            display.setCursor(38,30);
            display.setFont(&FreeSansBold12pt7b);
            display.setTextColor(1);
            display.print(cursor_val_int);
            if(left==0 && right==0){
              count = 0;
            }
            //ボタンの状態表示
            if(left != 0){
              display.fillTriangle(20,32,25,27,25,37,1);      
              is_changed = false; 
              count++;
            }    
            if(right != 0){
              display.fillTriangle(108,32,98,27,98,37,1);
              is_changed = false;
              count++;
            }
            //決定
            if(enter==1){
              val = cursor_val_int;  
            }
            if(enter!=0 && is_changed == false){
              display.setCursor(68,52);
              display.setFont();
              display.setTextSize(1);
              display.print("changed!");    
              is_changed = true;
            }
            //カーソル移動
            //左
            if(left==1)
              cursor_val_int = ((cursor_val_int + max - 1) % max);
            //右
            if(right==1)
              cursor_val_int = ((cursor_val_int + 1) % max);
            //左(加速)
            if(left==2){
              if(count > 20){
                cursor_val_int = ((cursor_val_int + max - 2) % max);
                sound.cursor();
              }
              else{
                count++;
              }
            }
            //右(加速)
            if(right==2){
                if(count > 20){
                  cursor_val_int = ((cursor_val_int + 2) % max);
                  sound.cursor();
                }
                else{
                  count++;
                }
                
            }
              
            //ホームに戻る
            if(back!=0){app_state = 0;}
        }

        void UI::changeBoolVal(const char* name,bool &val,bool default_val){
            display.clearDisplay();
            display.setCursor(0,0);
            display.setFont();
            display.setTextSize(1);
            display.setTextColor(1);
            display.print("change bool:[ ");
            display.print(name);
            display.print(" ]");
            display.setCursor(58,20);
            display.setFont(&FreeSans12pt7b);
            if(val == true){
              display.print("true"); 
            }
            else{
              display.print("false");
            }
            //ボタンの状態表示 
            if(right != 0 || left != 0){
              display.fillCircle(120,32,5,1);
            }
            //決定
            if(enter==1){
              val = cursor_val_int;  
            }
            if(enter!=0){
              display.setCursor(50,120);
              display.setFont();
              display.setTextSize(1);
              display.print("changed!");    
            }
            //カーソル移動
            if(left==1 || right == 1)
              cursor_val_bool = cursor_val_bool;
            //ホームに戻る
            if(back!=0){app_state = 0;}
        }
        

