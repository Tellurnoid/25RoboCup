#include <EEPROM.h>
#include <stddef.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerifBoldItalic12pt7b.h>//起動画面
#include <Fonts/FreeSansBoldOblique9pt7b.h>//起動画面(候補２。決まったら一方消してよし)
#include <Fonts/FreeMono9pt7b.h>//見出しなど
#include <Fonts/FreeSans9pt7b.h>//値調整UI用

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define RightPin A1
#define LeftPin  A0
#define OKPin    A3
#define BackPin  A2
#define SPKPin   6
 /*
文字サイズと使用するピクセル
TextSize 1 → 6×8
TextSize 2 → 12×16
TextSize 3 → 18×24
TextSize 4 → 24×32

*/
const char* app1Name = "LED Power";
const char* app2Name = "App2";
const char* app3Name = "App3";
const char* app4Name = "App4";
const char* app5Name = "App5";

volatile int app=0;//割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
int cursor=0;
int HowManyApps=5;

int LEDBrightness=80;

//型を定義 EEPROMに書き込む変数を増やす場合はここに定義
struct DataFormat {
  char* ROM_teamName;
  int ROM_LED_output;
};
DataFormat myData;//型を使ってEEPROM用の変数を作成
//DataFormat myData = {"URAWA BEOLSAE (2025)",255,1,3.14};// ←初期値入れる場合

void setup() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 ERROR"));
    for(;;); 
}
  display.clearDisplay();
  randomSeed(analogRead(0));  
  startUpShow();
  //delay(5);
  //display.setFont(NULL);
  display.setTextSize(1);
  printString_char("URAWA BEOLSAE 2025",0,64-8*4);
  printString_char("Tatsuki Sakuya Daichi",0,64-8*3);

  printString_char("input pin...",0,64-8*2);
  pinMode(RightPin, INPUT_PULLUP);
  pinMode(LeftPin, INPUT_PULLUP);
  pinMode(OKPin, INPUT_PULLUP);
  pinMode(BackPin, INPUT_PULLUP);
  printString_char("setup done!",0,64-8);
  if(!(IsPress_OK() || IsPress_Back() || IsPress_Right() || IsPress_Left())){
    while(!(IsPress_OK() || IsPress_Back() || IsPress_Right() || IsPress_Left()));
  }
  //draw_HomeMenu();
}


void loop(){
    if(app==0){homemenu();}
    else if(app==1){app1();}
    else if(app==2){app2();}
    else if(app==3){app3();}
    else if(app==4){app4();}
    else if(app==5){app5();}
    //delay(5); //フリーズすることが多ければ試して
}

