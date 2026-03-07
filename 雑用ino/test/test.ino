#include <Arduino.h>
void setup(){
    Serial.begin(115200);
}

void loop(){
   // Serial.println(analogRead(28));
   analogWrite(15, 255);
}



/*
ESP32

{
    "port": "/dev/ttyUSB0",
    "board": "esp32:esp32:esp32",
    "sketch": "RCJ_Japan_Keeper.ino"
}



*/