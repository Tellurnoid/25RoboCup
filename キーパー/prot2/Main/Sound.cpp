#include "Instance.h"
#include "Sound.h"

void Sound::init(){
    pinMode(beep, OUTPUT);
}

void Sound::success() {
  digitalWrite(beep, HIGH);
  delay(50);
  digitalWrite(beep, LOW);
  delay(50);
  digitalWrite(beep, HIGH);
  delay(30);
  digitalWrite(beep, LOW);      
}

void Sound::single() {
  digitalWrite(beep, HIGH);
  delay(50);
  digitalWrite(beep, LOW);
}
void Sound::user(int your_tone,int length_on, int length_off,int times){
        
        uint32_t                  last_time = 0;
  for(int i=0;i<times;i++){
      if ((millis() - last_time) >= length_on) {
        last_time = millis();
        tone(beep,your_tone,length_on);
      }
      else{
        digitalWrite(beep, LOW);
      }
  }
}
void Sound::calibrate(){
  user(1000,50,80,1);
}

void Sound::update() {
  if (state.sp.gyro_calibration == 1) {
    calibrate();
  }
  else digitalWrite(beep, LOW);
  //Serial.println(state.sp.gyro_calibration);
}