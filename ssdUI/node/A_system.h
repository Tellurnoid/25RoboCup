#ifndef A_SYSTEM_h
#define A_SYSTEM_h
#include <Arduino.h>//.hファイルではuint8_tを使うために必要
#include <U8x8lib.h>


const char* appName[] = { "back To Game","Serial Monitor", "vals" , "Line Sensor", "Bools" };

#define UpPin A1
#define DownPin A0
#define OKPin A3
#define BackPin A2
#define SPKPin 6

volatile uint8_t appState = 0;  //割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
uint8_t  cursor = 0;

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
DataFormat myData;
DataFormat readData;

class UIsystem{
  public:
   UIsystem();
   void initButtons();
   bool IsPress_UP();
   bool IsPress_Down();
   bool IsPress_Back();
   bool IsPress_OK();  
   
   void decidedSound();
   void cancelSound();
   void cursorSound();
   void errorSound();
  
  private:
     // Use the global `u8x8` defined in node.ino (declared extern in app.h).
     // Do not define another U8X8 object here to avoid multiple-definition
     // and macro-expansion issues. Access `u8x8` directly from implementation.
};
#endif