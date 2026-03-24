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

  //      static constexpr uint8_t NUM_APP = 7;
        UI::States UI::app[] = {
            {0,"Home",&UI::app_home},
            {1,"Game",&UI::app_game},
            {2,"Gyro",&UI::app_gyro},
            {3,"Line",&UI::app_line},
            {4,"Ball",&UI::app_ball},
            {5,"Echo",&UI::app_echo},
            {6,"Camera",&UI::app_cam},
            {7,"Kicker",&UI::app_kicker}
        };
        // 要素数
        const size_t UI::NUM_APP = sizeof(UI::app) / sizeof(UI::app[0]);

    void UI::init() {
      Wire.setSDA(4);
      Wire.setSCL(5);
      Wire.begin();
      Wire.setClock(400000);   // 400kHz
      if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        while (true);
      }

      Serial.begin(115200);
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

      //配列のアプリ実行
      //app[app_state].func();
      (this->*app[app_state].func)();

      // if(left==1 || right==1)
      //   sound.cursor();
      // if(enter==1)
      //   sound.cursor();
      // if(back==1)
      //   sound.back();
        
      // display.setFont();
      // display.setTextSize(1);
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

      for(int i=0; i<NUM_APP; i++){
        display.drawCircle((64/NUM_APP) + i * 128/NUM_APP, 50, (64/NUM_APP) -2, 1);
      }

      if(cursor_home==0){
        app_logo();
      }
      else{
        display.print(app[cursor_home].name);
         display.fillCircle((64/NUM_APP) + cursor_home * 128/NUM_APP, 50, (64/NUM_APP) -2, 1);
      }
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
          app_state = cursor_home;
          sound.enter();
        }
      }
      //カーソル移動
      //左
      if(left==1){
        cursor_home = ((cursor_home + NUM_APP - 1) % NUM_APP);
        sound.cursor();
      }

      //右
      if(right==1){
        cursor_home = ((cursor_home + 1) % NUM_APP);
        sound.cursor();
      }

      //ホーム画面で戻るキーは無効なのでエラー音など
      if(back==1){
          app_state = 1;
          sound.enter();
      }
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
        display.print("DIFENSE");
      }
      if(enter!=1 && is_on_game ==true){
        display.fillRect(0,0,128,64,1);
       // display.clearDisplay();
        display.setCursor(18,28);
        display.setTextColor(0);
        if(cursor_val_int == 0){
          display.print("ATACK");
        }
        else if(cursor_val_int == 1){
          display.print("DIFENSE");
        }
      }



      Serial.print(right);
      Serial.print(",");
      Serial.println(long_press);
      //カーソル移動
      if(left ==1 || right ==1){
        long_press = 0;
        if(is_on_game){
          is_on_game = false;
          command.sendCommand(MAIN, START, 0);
          sound.back();
        }
      }
      else if(left == 2){
        display.fillRect(63,60,3,4,1);
        long_press = long_press + 1.00f;
        display.fillRect(64,60,64 * long_press / change_time, 4 ,1);
        display.fillRect(64 - 64 * long_press / change_time,60,64 * long_press / change_time, 4 ,1);
      }
      else if(right == 2){
        display.fillRect(63,60,3,4,1);
        long_press = long_press + 1.00f;
        display.fillRect(64 * long_press / change_time,60,64 - (64 * long_press / change_time), 4 ,1);
        display.fillRect(64,60,64 -(64 * long_press / change_time), 4 ,1);
      }
      else if(left == 3 || right == 3){

      }
      else{
        long_press = 0;
      }

      //左
      if(left==2 && long_press == change_time){
        if(is_on_game){
          is_on_game = false;
          command.sendCommand(MAIN, START, 0);
          sound.back();
         }
         else{
          cursor_val_int = ((cursor_val_int + NUM_MODE - 1) % NUM_MODE);
          sound.cursor();
          display.drawRect(0,0,128,64,1);
         }
        long_press = 0;
        }
      //右
      if(right==2 && long_press == change_time){
          if(is_on_game){
            is_on_game = false;
            command.sendCommand(MAIN, START, 0);
            sound.back();
          }
          else{
            cursor_val_int = ((cursor_val_int + 1) % NUM_MODE);
            sound.cursor();
            display.drawRect(0,0,128,64,1);
          }
          long_press = 0;
        }
      if(enter==1){
          if(is_on_game == false){
            //ここに試合開始コマンド
              if(cursor_val_int == 0){
                command.sendCommand(MAIN, START, 1);
                sound.start();
                is_on_game = true;
              }
              else if(cursor_val_int == 1){
                command.sendCommand(MAIN, START, 2);
                sound.start();
                is_on_game = true;
            }
          }
          else{
            is_on_game = false;
            command.sendCommand(MAIN, START, 0);
            sound.back();
          }
          long_press = 0;
      }
      if(back!=0){
        if(is_on_game == true){
          is_on_game = false;
          command.sendCommand(MAIN,START,0);
        }
        else{
          app_state = 0;
          display.fillRect(0,62,128,64,1);
        }
         sound.back();
      }
    }
    // void UI::app_game(){
    //   display.clearDisplay();
    //   display.setCursor(18,28);
    //   display.setFont(&FreeSans9pt7b);
    //   display.setTextColor(1);
    //   if(cursor_val_int == 0){
    //     display.print("ATACK");
    //   }
    //   else if(cursor_val_int == 1){
    //     display.print("DIFENCE");
    //   }
    //   if(enter=2 && is_on_game ==true){
    //     display.fillRect(0,0,128,64,1);
    //   }

    //   bool any_key = (back || enter || right || left);
    //   if (is_on_game && any_key) {
    //     command.sendCommand(MAIN, START, 0);
    //     is_on_game = false;
    //     sound.back();
    //   }
    //   //カーソル移動
    //   //左
    //   if(left==1){
    //     cursor_val_int = ((cursor_val_int + NUM_MODE - 1) % NUM_MODE);
    //     sound.cursor();
    //     is_on_game = false;
    //     }
    //   //右
    //   if(right==1){
    //     cursor_val_int = ((cursor_val_int + 1) % NUM_MODE);
    //     sound.cursor();
    //     is_on_game = false;
    //     }
    //   if(back!=0){
    //     app_state = 0;
    //     is_on_game = false;
    //     display.fillRect(0,62,128,64,1);
    //   }
    //   if(enter==1){
    //     //ここに試合開始コマンド
    //     if (is_on_game == false && cursor_val_int == 0) {
    //       command.sendCommand(MAIN, START, 1);
    //       sound.start();
    //       is_on_game = true;
    //     }
    //     else if (is_on_game == false && cursor_val_int == 1) {
    //       command.sendCommand(MAIN, START, 2);
    //       sound.start();
    //       is_on_game = true;
    //     }
    //   else
    //     {
    //       command.sendCommand(MAIN, START, 0);
    //       is_on_game = false;
    //       sound.back();
    //     }
    //   }
    // }

void UI::app_kicker(){
  display.clearDisplay();
  display.setCursor(0,20);
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(1);
  display.print("Kicker");
  display.setFont(&FreeSans12pt7b);
  display.setCursor(60,40);
  display.print(data.dp.robot_angle);
  if(enter==1){
    command.sendCommand(MAIN, KICK, 1);
    sound.start();
  }
  if(back!=0){
    app_state = 0;
    display.fillRect(0,62,128,64,1);
    sound.back();
  }


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
            display.print("charge");
            display.setCursor(radius_mini*4,29);
            display.print("charge");
            display.setCursor(radius_mini*4,13);
            display.print("kick");
            display.setCursor(0,53);
            display.print("enter to execute");
            if(enter!=0){
              display.drawCircle(80,22+height_enter,radius_enter,1);
            }
            if(left!=0){
                display.drawCircle(5+radius_mini,16+height_mini-radius_mini,radius_mini,1);
            }
            if(right!=0){
                display.drawCircle(5+radius_mini,38+height_mini-radius_mini,radius_mini,1);
            }
  // // (トリガー(コマンドなら1,)
  // simpleSwitch(1,&command ,"kicker",&Command::sendCommand(MAIN, KICK, 1),"charge",&Sound::enter,"charge",&Sound::error);
  //       //ホームに戻る
  //       if(back!=0){
  //           app_state = 0;
  //           sound.back();
  //       }
  }
void UI::app_logo(){
  display.clearDisplay();
  // display.setCursor(0,40);
  // display.setFont(&FreeSans9pt7b);
  // display.setTextColor(1);
  // display.print("URAWA BEOLSAE");
    display.drawBitmap(0, 24, URAWA_BEOLSAE_logo, 128, 16, 1);
    for (int i=0; i<25; i++){
    display.drawPixel(random(0,128),random(0,64),1);
    }
  //  display.drawCircle(random(0,128),random(0,64),random(20,64),1);
  if(back!=0){
    app_state = 0;
    display.fillRect(0,62,128,64,1);
  }
}

void UI::app_line(){
  display.clearDisplay();
  display.setCursor(0,20);
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(1);
  display.print("Line");
  display.setFont(&FreeSans12pt7b);
  display.setCursor(60,40);
  display.print(data.dp.line_angle);
  if(enter==1){
    command.sendCommand(SUB, LINE_CALIBRATE, 1);
    sound.start();
  }
  if(enter==2){
    display.setCursor(0,10);
    display.setFont();
    display.print("Calibrate Start!");
  }
  if(back!=0){
    app_state = 0;
    display.fillRect(0,62,128,64,1);
    sound.back();
  }
}
void UI::app_ball(){
  display.clearDisplay();
  display.setCursor(0,20);
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(1);
  display.print("Ball");
  display.setFont(&FreeSans12pt7b);
  display.setCursor(60,40);
  display.print(data.dp.ball_angle);
  if(back!=0){
    app_state = 0;
    display.fillRect(0,62,128,64,1);
    sound.back();
  }
}
void UI::app_gyro(){
  display.clearDisplay();
  display.setCursor(0,20);
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(1);
  display.print("Gyro");
  display.setFont(&FreeSans12pt7b);
  display.setCursor(60,40);
  display.print(data.dp.robot_angle);
  if(enter==1){
    command.sendCommand(MAIN, RESET_GYRO, 1);
    sound.start();
  }
  if(back!=0){
    app_state = 0;
    display.fillRect(0,62,128,64,1);
    sound.back();
  }
}
void UI::app_echo(){
for(int i=0; i<8; i++){
  ave[i] = ave[i]*(1 - new_data_ratio) + data.dp.echoValues[i]*new_data_ratio;
}
  display.clearDisplay();
  display.setFont(&FreeSans12pt7b);
  display.setTextColor(1);
  display.setCursor(0,18);
  display.print("Echo");
  display.setFont();
  display.setCursor(0,29);
  for(int i=0; i<4; i++){
    display.print(i);
    display.print(": ");
    display.println(ave[i]);
  }
  for(int i=4; i<8; i++){
    display.setCursor(64,29+8*(i-4));
    display.print(i);
    display.print(": ");
    display.println(ave[i]);
  }
  if(back!=0){
    app_state = 0;
    display.fillRect(0,62,128,64,1);
    sound.back();
  }
}
void UI::app_cam(){
  ang_blue = data.dp.goal_angle;
  cd_blue = data.dp.goal_distance;
  
  display.clearDisplay();
  display.setCursor(0,10);
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(1);
  display.print("Camera");
  display.setCursor(0,43);
  display.print("ang: ");display.print(ang_blue);
  display.setCursor(0,58);
  display.print(" dis: ");display.print(cd_blue);
  // display.setCursor(64,43);
  // display.print("ang: ");display.print(ang_yellow);
  // display.setCursor(64,58);
  // display.print(" dis: ");display.print(cd_yellow);

  display.drawLine(0,29,128,29,1);
  display.setFont();
  display.setCursor(0,21);
  display.print("Blue");
  display.setCursor(64,21);
  display.print("Yellow");
  
  if(back!=0){
    app_state = 0;
    display.fillRect(0,62,128,64,1);
    sound.back();
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


