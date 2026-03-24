#include "sound.h"
#include <Arduino.h>

void Sound::init(){
    pinMode(27, OUTPUT);
}
void Sound::single(){
  tone(pin,1000);
  delay(50);
  digitalWrite(pin, LOW);
}
void Sound::enter(){
  tone(pin,900,50);
  delay(50);
  tone(pin,1000,30);
  delay(50);
  tone(pin,2000,30);
}
void Sound::error(){
  tone(pin,700,50);
  delay(50);
  tone(pin,700,50);
}
void Sound::cursor(){
  tone(pin,2000,50);
}
void Sound::back(){
  tone(pin,2000,50);
  delay(50);
  tone(pin,1000,30);
}
void Sound::success(){
  tone(pin,900);
  delay(100);
  tone(pin,1000);
  delay(100);
  tone(pin,2000,100);
}
void Sound::start(){
  tone(pin,2000,500);
}
void Sound::user(int your_tone,int length_on, int length_off,int times){
  for(int i=0;i<times;i++){
    tone(pin,your_tone,length_on);
    unsigned long start = millis();
    while (millis() - start < length_off) {}
  }
}