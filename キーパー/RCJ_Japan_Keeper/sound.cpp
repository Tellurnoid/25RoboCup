#include <Arduino.h>
#include "sound.h"


    void Sound::init(){
        pinMode(beep, OUTPUT);
    }
    void Sound::successSound(){
      digitalWrite(beep, HIGH);
      delay(50);
      digitalWrite(beep, LOW);
      delay(50);
      digitalWrite(beep, HIGH);
      delay(30);
      digitalWrite(beep, LOW);      
    }
    void Sound::single(){
       digitalWrite(beep, HIGH);
       delay(50);
       digitalWrite(beep, LOW);
    }