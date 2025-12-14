#include <ssdUI_vals.h>
//#include <EEPROM.h>
//#include <stddef.h>
//#include <Tiny4kOLED.h>
/*
軽量化メモ
・Serial.print("ABC"); ⇒　Serial.print(F("ABC"));
*/
#define screenWidth 128
#define screenHeight 64

#define UpPin A1
#define DownPin A0
#define OKPin A3
#define BackPin A2
#define SPKPin 6

const char* appName[] = { "Home", "LED Power", "SerialMonitor", "Bools", "App3", "App4", "Costom1", "Costom2" };

volatile uint8_t app = 0;  //割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
uint8_t cursor = 0;
uint8_t howManyApps = 7;

bool BoolVals[4]={0,0,0,0};




//ボタン
bool IsPress_UP(){
  return digitalRead(UpPin) == LOW;
}
bool IsPress_Down(){
  return digitalRead(DownPin) == LOW;
}
bool IsPress_OK(){
  return digitalRead(OKPin) == LOW;
}
bool IsPress_Back(){
  return digitalRead(BackPin) == LOW;
}

//音
void decidedSound(){
  tone(SPKPin, 550, 50);
  delay(70);
  tone(SPKPin, 750, 50);
  delay(70);
  tone(SPKPin, 900, 100);
}
void cancelSound(){
  tone(SPKPin,550,50);
  delay(80);
  tone(SPKPin,110,100);
}
void cursorSound(){
  tone(SPKPin, 440, 20);
}
void errorSound(){
  tone(SPKPin,110,100);
}

//ホームメニュー
void preview_homemenu(){}
void homemenu(){
  //カーソル下移動
  if(IsPress_Down()){
    cursorSound();
    cursor=cursor+1;
    if(cursor>HowManyApps){
      cursor=0;
    }
    myData.ROM_cursor = cursor;
    EEPROM.put(offsetof(DataFormat,ROM_cursor),myData.ROM_cursor);
    while(IsPress_Down());
    preview_homemenu();
  }
  //カーソル上移動
  else if(IsPress_UP()){
    cursorSound();
    cursor=cursor-1;
    if(cursor<0){
      cursor=HowManyApps;
    }
     myData.ROM_cursor = cursor;
     EEPROM.put(offsetof(DataFormat,ROM_cursor),myData.ROM_cursor);
    while(IsPress_UP());
    preview_homemenu();
  }
  //Okが押されたアプリを開く
  else if(IsPress_OK()){
    if(cursor==0){
      errorSound();
    }
    else{
      decidedSound();
      app=cursor;
      switch(cursor){
        case 1:
          //draw_app1();
          break;
        case 2:
          //draw_app2();
          break;
        case 3:
          app=3;
         // draw_app3();
          break;
        case 4:
          //draw_app5();
          break;
        case 5:
         // draw_app6();
          break;
        case 6:
         // draw_app_costoms(1);
          break;
        case 7:
         // draw_app_costoms(2);
          break;
        }
       }
  } 
}

