#pragma once
#ifndef D_APP_H
#define D_APP_H
#include <Arduino.h>
#include <U8x8lib.h>
#include "A_system.h"
#include "C_draw.h"


// Declare the global display object (defined once in node.ino)
extern U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8;
// グローバル実体は1箇所に定義します（A_system.cppなど）
// extern app app;
class draw;
class App{
    public:
      void homeMenu();
      void LEDBrightness();
      void SerialMonitor();
private:
    UIsystem* ui_;
    Draw* draw_;
};

extern App app;
#endif