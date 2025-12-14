#include <ssdUI_vals.h>

void setup(){
  Serial.begin(9600);
  oled.begin(screenWidth, screenHeight, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);  
  randomSeed(analogRead(0));
  startUpShow();
  pinMode(UpPin, INPUT_PULLUP);
  pinMode(DownPin, INPUT_PULLUP);
  pinMode(OKPin, INPUT_PULLUP);
  pinMode(BackPin, INPUT_PULLUP);
  DataFormat readData;
  EEPROM.get(0, readData);
  cursor = readData.ROM_cursor;

  if (!(IsPress_OK() || IsPress_Back() || IsPress_UP() || IsPress_Down())) {
    while (!(IsPress_OK() || IsPress_Back() || IsPress_UP() || IsPress_Down()));
  }
}
void loop()
{
  if (app == 0) {
    homemenu();
  } else if (app == 1) {
    app1();
  } else if (app == 2) {
    
  } else if (app == 3) {
    
  } else if (app == 4) {
    
  } else if (app == 5) {
    
  } else if (app == 6) {
    app_costoms(1);
  } else if (app == 7) {
    app_costoms(2);
  }
}
