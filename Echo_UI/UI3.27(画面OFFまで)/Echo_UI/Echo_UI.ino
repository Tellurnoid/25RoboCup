#include "Instance.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>   //起動画面(候補２。決まったら一方消してよし)
#include <Fonts/FreeMono9pt7b.h>              //見出しなど
#include <Fonts/FreeSans9pt7b.h>              //値調整UI用
// #include "UART.h"
// #include "echo.h"
// #include "button.h"
// //#include "commands.h"
// #include "UI.h"
// #include "sound.h"

// UI ui;
// Echo echo;
// Button button;    
// Sound sound;
//Command command;
void setup() {
//  uart.init();
  button.init();
  ui.init();
  ui.startUp();
  sound.init();
  sound.success();
  // Serial.begin(115200);
  uart.initUART();
}

void loop() {
  uart.updateUART();
  data.updateData();
  ui.main();
}

