

//ボタン
bool IsPress_UP(){
  return digitalRead(UpPin) == LOW;
}
bool IsPress_Down(){
  return digitalRead(DownPin) == LOW;
}
bool IsPress_OK(){
  return digitalRead(OKPin) == LOW;
}
bool IsPress_Back(){
  return digitalRead(BackPin) == LOW;
}

//音
void decidedSound(){
    tone(SPKPin, 550, 50);
    delay(70);
    tone(SPKPin, 750, 50);
    delay(70);
    tone(SPKPin, 900, 100);
}
void cancelSound(){
      tone(SPKPin,550,50);
      delay(80);
      tone(SPKPin,110,100);
}
//ホームメニュー
void preview_homemenu(){}
void homemenu(){}

