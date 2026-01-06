#include "A_system.h"
#include <U8x8lib.h>  

UIsystem::UIsystem(){}
//ボタンが押されたかの判定
bool UIsystem::IsPress_UP(){
  return digitalRead(UpPin) == LOW;
}
bool UIsystem::IsPress_Down(){
  return digitalRead(DownPin) == LOW;
}
bool UIsystem::IsPress_OK(){
  return digitalRead(OKPin) == LOW;
}
bool UIsystem::IsPress_Back(){
  return digitalRead(BackPin) == LOW;
}

void UIsystem::initButtons(){
    pinMode(UpPin, INPUT_PULLUP);
    pinMode(DownPin, INPUT_PULLUP);
    pinMode(OKPin, INPUT_PULLUP);
    pinMode(BackPin, INPUT_PULLUP);
}

//音
void UIsystem::decidedSound(){
  tone(SPKPin, 550, 50);
  delay(70);
  tone(SPKPin, 750, 50);
  delay(70);
  tone(SPKPin, 900, 100);
}
void UIsystem::cancelSound(){
  tone(SPKPin,550,50);
  delay(80);
  tone(SPKPin,110,100);
}
void UIsystem::cursorSound(){
  tone(SPKPin, 440, 20);
}
void UIsystem::errorSound(){
  tone(SPKPin,110,100);
}
