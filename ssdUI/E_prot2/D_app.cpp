#include <U8x8lib.h>  
#include <EEPROM.h>
#include "A_system.h"
#include "C_draw.h"
#include "D_app.h"
App app;

void App::homeMenu(){
  uint8_t HowManyApps = 5;
  //カーソル下移動
  if(uisystem.IsPress_Down()){
    uisystem.cursorSound();
    cursor=cursor+1;
    if(cursor>HowManyApps){
      cursor=0;
    }
    myData.ROM_cursor = cursor;
    EEPROM.put(offsetof(DataFormat,ROM_cursor),myData.ROM_cursor);
    while(uisystem.IsPress_Down());
    drawObj.preview_homemenu();
  }
  //カーソル上移動
  else if(uisystem.IsPress_UP()){
    uisystem.cursorSound();
    cursor=cursor-1;
    if(cursor<0){
      cursor=HowManyApps;
    }
     myData.ROM_cursor = cursor;
     EEPROM.put(offsetof(DataFormat,ROM_cursor),myData.ROM_cursor);
    while(uisystem.IsPress_UP());
    drawObj.preview_homemenu();
  }

}
void App::LEDBrightness(){}
void App::SerialMonitor(){}
