#include "A_system.h"
#include "draw.h"
#include "app.h"
#include <U8x8lib.h>  
#include <EEPROM.h>
UIsystem UIsystem;//A_system.hの中身
draw draw;

void app::homeMenu(){
  uint8_t HowManyApps = 5;
  //カーソル下移動
  if(UIsystem.IsPress_Down()){
    UIsystem.cursorSound();
    cursor=cursor+1;
    if(cursor>HowManyApps){
      cursor=0;
    }
    myData.ROM_cursor = cursor;
    EEPROM.put(offsetof(DataFormat,ROM_cursor),myData.ROM_cursor);
    while(UIsystem.IsPress_Down());
    draw.preview_homemenu();
  }
  //カーソル上移動
  else if(UIsystem.IsPress_UP()){
    UIsystem.cursorSound();
    cursor=cursor-1;
    if(cursor<0){
      cursor=HowManyApps;
    }
     myData.ROM_cursor = cursor;
     EEPROM.put(offsetof(DataFormat,ROM_cursor),myData.ROM_cursor);
    while(UIsystem.IsPress_UP());
    draw.preview_homemenu();
  }

}
void app::LEDBrightness(){}
void app::SerialMonitor(){}