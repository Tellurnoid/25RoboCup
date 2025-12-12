#include "draw.h"
#include "app.h"
//#include <U8x8lib.h>
 //U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

uint8_t app = 0;

void setup(){
  u8x8.begin();
  u8x8.setFlipMode(0);
  draw.startupShow();
}

void homeMenu(){
  u8x8.setCursor(0,10);
  u8x8.setFont(u8x8_font_inr21_2x4_r);
  u8x8.print("-----------------------------------------");//https://lang-ship.com/blog/work/grove-beginner-kit-08-oled/
}
void app_LEDBrightness(){}
void app_SerialMonitor(){}

void loop(){
  if(app==0){
    homeMenu();
  }
  else if(app==1){
    app_LEDBrightness();
  }
  else if(app==2){
    app_SerialMonitor();
  }
}
