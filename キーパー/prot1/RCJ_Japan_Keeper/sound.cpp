#include <Arduino.h>
#include "Instance.h"

    void Sound::init(){
        pinMode(beep, OUTPUT);
    }
    void Sound::success(){
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