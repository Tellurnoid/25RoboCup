#include<Arduino.h>
#include <U8x8lib.h>
#include "A_system.h"
#include "draw.h"
#include "app.h"

draw::draw(){}
void draw::drawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,bool color){}
void draw::fillRect(uint8_t x,uint8_t y, uint8_t w, uint8_t t, bool color){}
void draw::triangle(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t x3,uint8_t y3,bool color){}
void draw::fillTriangle(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t x3,uint8_t y3,bool color){}


void draw::startupShow(){
  u8x8.setCursor(1,10);
  u8x8.setFont(u8x8_font_inr21_2x4_r);
  u8x8.print("BEOLSAE");//https://lang-ship.com/blog/work/grove-beginner-kit-08-oled/
}

void draw::homeMenu(){
  u8x8.clear();
  u8x8.setCursor(0,10);
  u8x8.setFont(u8x8_font_inr21_2x4_r);
  u8x8.print("-----------------------------------------");//https://lang-ship.com/blog/work/grove-beginner-kit-08-oled/

}
void draw::preview_homemenu(){

}

void draw::draw_LEDBrightness(){}
void draw::draw_serialMonitor(){}