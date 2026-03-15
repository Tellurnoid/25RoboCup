#include <Arduino.h>
#include "Instance.h"
#include "keeper_main.h"

Keeper keeper;
void setup(){
    keeper.init();
}

void loop(){
    keeper.updateAllData();
    keeper.main();
}