#ifndef SYSTEMS_H
#define SYSTEMS_H

#define UpPin A1
#define DownPin A0
#define OKPin A3
#define BackPin A2
#define SPKPin 6


//extern int stickX;
//extern int stickY;
const char* appName[]={"Home","LED Power","Bools","App3","App4","SerialMonitor","Costom1","Costom2"};
extern volatile uint8_t app;  //割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
extern uint8_t cursor;
extern const uint8_t HowManyApps;
extern uint8_t LEDBrightness;

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
extern DataFormat myData;  //型を使ってEEPROM用の変数を作成

#endif