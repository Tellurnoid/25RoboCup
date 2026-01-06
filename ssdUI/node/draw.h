#ifndef DRAW_h
#define DRAW_h
#include <Arduino.h>//.hファイルではuint8_tを使うために必要
#include <U8x8lib.h>

class draw{
  public:
     draw();
     void drawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,bool color);
     void fillRect(uint8_t x,uint8_t y, uint8_t w, uint8_t t, bool color);
     void triangle(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t x3,uint8_t y3,bool color);
     void fillTriangle(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t x3,uint8_t y3,bool color);

     void startupShow();
     void homeMenu();
     void preview_homemenu();
     void draw_LEDBrightness();
     void draw_serialMonitor();
  
  private:
     // Use the global `u8x8` defined in node.ino (declared extern in app.h).
     // Do not define another U8X8 object here to avoid multiple-definition
     // and macro-expansion issues. Access `u8x8` directly from implementation.
};
#endif