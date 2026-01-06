#ifndef SSDUI_VALS_H
#define SSDUI_VALS_H

#include <EEPROM.h>
#include <stddef.h>
#include <Tiny4kOLED.h>

const char* appName[] = { "Home", "LED Power", "SerialMonitor", "Bools", "App3", "App4", "Costom1", "Costom2" };

volatile uint8_t app;  //割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
uint8_t cursor;
uint8_t howManyApps;

bool BoolVals[4];

#define screenWidth 128
#define screenHeight 64

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
DataFormat writeData; 


#endif