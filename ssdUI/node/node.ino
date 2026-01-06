#include "A_system.h"
#include "draw.h"
#include "app.h"
#include <U8x8lib.h>
#include <EEPROM.h>

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);
UIsystem UIsystem;//A_system.hの中身
draw draw;
app app;

/*
メモ
ホーム
1:back to game (決定を押すと超音波センサーのUART送信開始)
2:Serial monitor
3:vals    intとfloatで、それぞれでもう一度分岐する
4:line sensor  キャリブレーション、全体調整、個別調整
5:bools     カメラ、超音波など一部機能の無効化

*/

void USsensor(){
  //超音波センサーの内容
}

bool IsOnGame = true; //trueだと超音波センサー,falseでUI起動
void setup(){
  UIsystem.initButtons();
  if(UIsystem.IsPress_UP() && UIsystem.IsPress_Down()){
    IsOnGame = false;
    UIsystem.decidedSound();
  }
  u8x8.begin();
  u8x8.setFlipMode(0);
  //EEPROM
  DataFormat readData;
  EEPROM.get(0, readData);
  cursor = readData.ROM_cursor;
  draw.startupShow();
  if (IsOnGame == false && !(UIsystem.IsPress_OK() || UIsystem.IsPress_Back() || UIsystem.IsPress_UP() || UIsystem.IsPress_Down()) ) {
    while (!(UIsystem.IsPress_OK() || UIsystem.IsPress_Back() || UIsystem.IsPress_UP() || UIsystem.IsPress_Down()));
  }
}

void loop(){
  if(IsOnGame == true){
    USsensor();
  }
  else{
    if(appState==0){
      app.homeMenu();
    }
    else if(appState==1){
      app.LEDBrightness();
    }
    else if(appState==2){
      app.SerialMonitor();
    }
  }
}
