#pragma once
#ifndef A_SYSTEM_h
#define A_SYSTEM_h
#include <Arduino.h>//.hファイルではuint8_tを使うために必要
#include <U8x8lib.h>

extern volatile int appState;  //割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
extern int cursor;
// extern UIsystem UIsystem;
//extern const char* appName[];
//extern UIsystem UIsystem;
#define UpPin A1
#define DownPin A0
#define OKPin A3
#define BackPin A2
#define SPKPin 6


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
//DataFormat writeData;
extern DataFormat myData;
//DataFormat readData;

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
// グローバル実体は1箇所に定義します（A_system.cppなど）
extern UIsystem uisystem;//A_system.hの中身
#endif