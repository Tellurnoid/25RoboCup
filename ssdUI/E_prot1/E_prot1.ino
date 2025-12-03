#include <EEPROM.h>
#include <stddef.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>   //起動画面(候補２。決まったら一方消してよし)
#include <Fonts/FreeMono9pt7b.h>              //見出しなど
#include <Fonts/FreeSans9pt7b.h>              //値調整UI用

#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define UpPin A1
#define DownPin A0
#define OKPin A3
#define BackPin A2
#define SPKPin 6
/*
文字サイズと使用するピクセル
TextSize 1 → 6×8
TextSize 2 → 12×16
TextSize 3 → 18×24
TextSize 4 → 24×32
*/
const char* appName[]={"Home","LED Power","Bools","App3","App4","SerialMonitor","Costom1","Costom2"};

volatile int app = 0;  //割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
int cursor = 0;
const int HowManyApps = 7;

int LEDBrightness = 80;

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
DataFormat myData;  //型を使ってEEPROM用の変数を作成
//DataFormat myData = {"URAWA BEOLSAE (2025)",255,1,3.14};// ←初期値入れる場合


void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 ERROR"));
    for (;;);
  }
  display.clearDisplay();
  randomSeed(analogRead(0));
  startUpShow();
  display.setFont(NULL);
  display.setTextSize(1);
  printString_char("URAWA BEOLSAE 2025", 0, 64 - 8 * 4);
  printString_char("Tatsuki Sakuya Daichi", 0, 64 - 8 * 3);
  printString_char("input pin...", 0, 64 - 8 * 2);
  pinMode(UpPin, INPUT_PULLUP);
  pinMode(DownPin, INPUT_PULLUP);
  pinMode(OKPin, INPUT_PULLUP);
  pinMode(BackPin, INPUT_PULLUP);
  DataFormat readData;
  EEPROM.get(0, readData);
  cursor = readData.ROM_cursor;
  printString_char("setup done!", 0, 64 - 8);

  if (!(IsPress_OK() || IsPress_Back() || IsPress_UP() || IsPress_Down())) {
    while (!(IsPress_OK() || IsPress_Back() || IsPress_UP() || IsPress_Down()));
  }
  //draw_HomeMenu();
}


void loop() {
  if (app == 0) {
    homemenu();
  } else if (app == 1) {
    app1();
  } else if (app == 2) {
    app2();
  } else if (app == 3) {
    app3();
  } else if (app == 4) {
    app4();
  } else if (app == 5) {
    app5();
  } else if (app == 6) {
    app_costoms(1);
  } else if (app == 7) {
    app_costoms(2);
  }
  //delay(5); //フリーズすることが多ければ試して
}
