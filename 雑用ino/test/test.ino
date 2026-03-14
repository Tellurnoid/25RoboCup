#include <Arduino.h>
void setup(){
    Serial.begin(115200);
}

void loop(){
    Serial.println(analogRead(28));
    analogWrite(21, 0);
   analogWrite(27, 255);
   delay(500);
   analogWrite(27, 0);
   delay(500);
}



/*
ESP32

{
    "port": "/dev/ttyUSB0",
    "board": "esp32:esp32:esp32",
    "sketch": "RCJ_Japan_Keeper.ino"
}


RP
{
    "port": "/dev/ttyADM0",
    "board": "rp2040:rp2040:rpipico2w",
    "sketch": "RCJ_Japan_Keeper.ino"
}

tasks
ESP32
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "compile",
      "type": "shell",
      "command": "arduino-cli compile --fqbn esp32:esp32:esp32 --verbose .",
      "group": "build"
    },
    {
      "label": "upload",
      "type": "shell",
      "command": "arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 --verbose .",
      "dependsOn": "compile"
    }
  ]
}

RP
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "compile",
      "type": "shell",
      "command": "arduino-cli compile --fqbn rp2040:rp2040:rpipico2w --verbose .",
      "group": "build"
    },
    {
      "label": "upload",
      "type": "shell",
      "command": "arduino-cli upload -p /dev/ttyACM0 --fqbn rp2040:rp2040:rpipico2w --verbose .",
      "dependsOn": "compile"
    }
  ]
}

*/