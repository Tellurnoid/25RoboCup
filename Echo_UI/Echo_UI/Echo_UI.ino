#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>   //起動画面(候補２。決まったら一方消してよし)
#include <Fonts/FreeMono9pt7b.h>              //見出しなど
#include <Fonts/FreeSans9pt7b.h>              //値調整UI用
//#include "UART.h"
#include "echo.h"
#include "button.h"
//#include "commands.h"
#include "UI.h"

UI ui;
Echo echo;
Button button;    
//Command command;
void setup() {
//  uart.init();
button.init();
  ui.init();
  ui.startUp();
  Serial.begin(115200);
}

void loop() {
 // uart.update();

//０が緑 押されたら1 長押し2
  //if (button.buttonStates[0] == 1) {
  //  command.command(MAIN, START, 1);//変数startを1に変更させるコマンド
  echo.read();
  ui.main();
}

