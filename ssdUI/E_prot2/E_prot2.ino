#include <EEPROM.h>
#include <stddef.h>
#include <Tiny4kOLED.h>
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

const char* appName[] = { "Home", "LED Power", "Bools", "App3", "App4", "SerialMonitor", "Costom1", "Costom2" };

volatile uint8_t app = 0;  //割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
uint8_t cursor = 0;
uint8_t howManyApps = 7;

//型を定義 EEPROMに書き込む変数を増やす場合はここに定義UpPin
struct DataFormat {
  int ROM_cursor;
  char* ROM_teamName;
  int ROM_LED_output;
  bool ROM_BoolVal1;
  bool ROM_BoolVal2;
  bool ROM_BoolVal3;
  long ROM_costom1;  //1 costom1 (int )
  long ROM_costom2;  //5 costom2  intは16bit（-32768〜32767）
};
DataFormat writeData;


void setup(){
  Serial.begin(9600);
  //ここにディスプレイ初期化関数
  randomSeed(analogRead(0));
  startUpShow();
  pinMode(UpPin, INPUT_PULLUP);
  pinMode(DownPin, INPUT_PULLUP);
  pinMode(OKPin, INPUT_PULLUP);
  pinMode(BackPin, INPUT_PULLUP);
  DataFormat readData;
  EEPROM.get(0, readData);
  cursor = readData.ROM_cursor;

  if (!(IsPress_OK() || IsPress_Back() || IsPress_UP() || IsPress_Down())) {
    while (!(IsPress_OK() || IsPress_Back() || IsPress_UP() || IsPress_Down()));
  }
}
void loop()
{
  if (app == 0) {
    homemenu();
  } else if (app == 1) {
    
  } else if (app == 2) {
    
  } else if (app == 3) {
    
  } else if (app == 4) {
    
  } else if (app == 5) {
    
  } else if (app == 6) {
    app_costoms(1);
  } else if (app == 7) {
    app_costoms(2);
  }
}
