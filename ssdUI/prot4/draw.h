#ifndef DRAW_h
#define DRAW_h
#include <Arduino.h>//.hファイルではuint8_tを使うために必要
#include <U8x8lib.h>

class draw{
  public:
     void drawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,bool color);
     void fillRect(uint8_t x,uint8_t y, uint8_t w, uint8_t t, bool color);
     void triangle(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t x3,uint8_t y3,bool color);
     void fillTriangle(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t x3,uint8_t y3,bool color);

     void startupShow();
     void draw_homeMenu();
     void draw_LEDBrightness();
     void draw_serialMonitor();
  
  private:
     U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

};
#endif