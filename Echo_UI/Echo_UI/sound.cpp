#include "sound.h"
#include <Arduino.h>

void Sound::init(){
    pinMode(27, OUTPUT);
}
void Sound::tone(){
  digitalWrite(pin, HIGH);
  delay(50);
  digitalWrite(pin, LOW);
}
void Sound::success(){
  digitalWrite(pin, HIGH);
  delay(50);
  digitalWrite(pin, LOW);
  delay(50);
  digitalWrite(pin, HIGH);
  delay(30);
  digitalWrite(pin, LOW);
}
void Sound::error(){}
void Sound::cursor(){
  digitalWrite(pin, HIGH);
  delay(30);
  digitalWrite(pin, LOW);
}
void Sound::back(){
  digitalWrite(pin, HIGH);
  delay(30);
  digitalWrite(pin, LOW);
  delay(20);
  digitalWrite(pin, HIGH);
  delay(30);
  digitalWrite(pin, LOW);
}