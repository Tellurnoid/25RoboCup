#include <EEPROM.h>
#include <stddef.h>

#include <Wire.h>

/*
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>   //起動画面(候補２。決まったら一方消してよし)
#include <Fonts/FreeMono9pt7b.h>              //見出しなど
#include <Fonts/FreeSans9pt7b.h>              //値調整UI用


#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
*/

#define UpPin A1
#define DownPin A0
#define OKPin A3
#define BackPin A2
#define SPKPin 6

const char* appName[]={"Home","LED Power","Bools","App3","App4","SerialMonitor","Costom1","Costom2"};

volatile int app = 0;  //割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
int cursor = 0;

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