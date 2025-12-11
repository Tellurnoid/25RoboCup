#include "systems.h"
#include <Arduino.h> 


//const char* appName[]={"Home","LED Power","Bools","App3","App4","SerialMonitor","Costom1","Costom2"};

volatile uint8_t app = 0;  //割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
uint8_t cursor = 0;
const uint8_t HowManyApps = 7;

uint8_t LEDBrightness = 80;