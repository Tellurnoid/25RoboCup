
void preview_setIntVal(){}
int app_setIntVal(uint8_t intValMin , uint8_t intValMax){
   const unsigned long longPressTime = 500; // 長押しとみなす時間（ミリ秒）
   uint16_t counter = 0;
   bool lastUPButtonState = LOW;
   bool lastDOWNButtonState = LOW;
   unsigned long buttonPressStart = 0;

  DataFormat myData;
    if(IsPress_UP() && !lastUPButtonState){
      buttonPressStart = millis();
      counter++; // 単押しとして1加算
      yourIntVal=yourIntVal+1;
      if(yourIntVal>yourIntValMax){
        yourIntVal=yourIntValMin;
      }
      preview_IntVal();
      display.fillTriangle(59, 23, 64, 18, 69, 23, SSD1306_WHITE);
      display.drawTriangle(59, 46, 64, 51, 69, 46, SSD1306_WHITE);
      display.display();
    }
    if (UPbuttonState) {// ボタンが押され続けている場合
      unsigned long pressDuration = millis() - buttonPressStart;
      if (pressDuration >= longPressTime) {
        // 長押し判定：押している間毎ループ加算
        counter++;
        yourIntVal=yourIntVal+1;
      if(yourIntVal>yourIntValMax){
        yourIntVal=yourIntValMin;
      }
      preview_IntVal();
      display.fillTriangle(61, 23, 64, 18, 67, 23, SSD1306_WHITE);
      display.drawTriangle(59, 46, 64, 51, 69, 46, SSD1306_WHITE);
      display.display();
        //delay(30);
      }
      
    }
    lastUPButtonState = UPbuttonState;


    //下
    bool DOWNbuttonState = digitalRead(DownPin) == LOW; // 押されているとき true
    if(IsPress_Down() && !lastDOWNButtonState){
      buttonPressStart = millis();
      counter++; // 単押しとして1加算
      yourIntVal=yourIntVal-1;
      if(yourIntVal<yourIntValMin){
        yourIntVal=yourIntValMax;
      }
      //while(IsPress_Down());
      preview_IntVal();
      display.drawTriangle(59, 23, 64, 18, 69, 23, SSD1306_WHITE);
      display.fillTriangle(59, 46, 64, 51, 69, 46, SSD1306_WHITE);
      display.display();

    }
    if (DOWNbuttonState) {// ボタンが押され続けている場合
      unsigned long pressDuration = millis() - buttonPressStart;
      if (pressDuration >= longPressTime) {
        // 長押し判定：押している間毎ループ加算
        counter++;
        yourIntVal=yourIntVal-1;
      if(yourIntVal<yourIntValMin){
        yourIntVal=yourIntValMax;
      }
      preview_IntVal();
      display.drawTriangle(61, 23, 64, 18, 67, 23, SSD1306_WHITE);
      display.fillTriangle(61, 47, 64, 52, 67, 47, SSD1306_WHITE);
      display.display();
        //delay(30);
      }
      
    }
    lastDOWNButtonState = DOWNbuttonState;

    if(!IsPress_Down() && !IsPress_UP()){
      display.fillTriangle(59, 23, 64, 18, 69, 23, 0);
      display.fillTriangle(59, 46, 64, 51, 69, 46, 0);
      display.drawTriangle(59, 23, 64, 18, 69, 23, 1);
      display.drawTriangle(59, 46, 64, 51, 69, 46, 1);
      display.display();
    }
    return yourIntVal;
}

void preview_setFloatVal(){}
int app_setFloatVal(){}

void app_costoms(uint8_t costoms){}



void app1(){
  sp0.UISendHead = 1;//LEDの送信用データのアドレスは1。式変える必要あり
  DataFormat myData;
  DataFormat readData;
  EEPROM.get(0,readData);

  if(IsPress_Back()){
   cursor=readData.ROM_cursor;
   cancelSound();
   //draw_homemenu(); //////////ホーム絵画いれるか検討
   preview_homemenu();
   app=0;
  }
  else if(IsPress_OK()){
    myData.ROM_LED_output = app_setIntVal(0,255);
    EEPROM.put(offsetof(DataFormat,ROM_LED_output),myData.ROM_LED_output);//書き込み
    decidedSound();
    uint8_t writtenIntData = myData.ROM_LED_output;
    //ここに saved as:(writtenIntData)を絵画
        if(IsPress_OK()){while(IsPress_OK());}
  }
  else{
    app_setIntVal(0,255);
  }
}

void app2(){
    DataFormat myData;
    DataFormat readData;
    EEPROM.get(0, readData);
    if(IsPress_Back()){
      myData.ROM_BoolVal1 = BoolVals[0];
      myData.ROM_BoolVal2 = BoolVals[1];
      myData.ROM_BoolVal3 = BoolVals[2];
      EEPROM.put(offsetof(DataFormat,ROM_BoolVal1),myData.ROM_BoolVal1);//EEPROMに書き込み
      EEPROM.put(offsetof(DataFormat,ROM_BoolVal2),myData.ROM_BoolVal2);//EEPROMに書き込み
      EEPROM.put(offsetof(DataFormat,ROM_BoolVal3),myData.ROM_BoolVal3);//EEPROMに書き込み
      cursor=readData.ROM_cursor;
      cancelSound();
     // draw_HomeMenu();
      inHomeMenu_AppPreview();
      app=0;
    }
    else if(IsPress_OK()){
      BoolVals[cursor_settingBool]=!BoolVals[cursor_settingBool]; 
      decidedSound();
      //以下のプレビューは関数化するorそのまま？
/*
      display.fillRect(128-6*5,8,128-6*5,64-8,0);
     for(int i=0; i<boolCursorMax; i++){
      //printString_char(BoolValsNames[i],15,8+i*8);
       if(BoolVals[i]==0){
        printString_char("TRUE",128-6*5,8+i*8);
       }
       else{
        printString_char("FALSE",128-6*5,8+i*8);
       }
*/
      }
    if(IsPress_OK()){while(IsPress_OK());}
    else{
       setYourVal_Bool();//値調整UIを回し続ける
   } 
}
//boolsはつかうときに開ける
/*
const char* BoolValsNames[] = {
  "BoolVal1",
  "BoolVal2",
  "BoolVal3"
};
// 2. 名前配列の要素数をコンパイル時に取得
constexpr int boolCursorMax = sizeof(BoolValsNames) / sizeof(BoolValsNames[0]);
// 3. BoolVals 配列を boolCursorMax で定義
bool BoolVals[boolCursorMax] = {false}; // 全要素を false で初期化
int cursor_settingBool = 0;

void setYourVal_Bool_preview(){
      tone(SPKPin, 440, 20);
      display.fillRect(0,0,128,8,0);
      display.setCursor(20,0);
      display.setTextSize(1);
      display.print(cursor_settingBool);
      display.print(":");
      display.print(BoolValsNames[cursor_settingBool]);
      display.fillRect(10,8,3,64,0);
      display.fillRect(10,8+cursor_settingBool*8,3,8,1);
      display.display();
}

int setYourVal_Bool(){//StructTypeには共通でmyDataを入れる
    DataFormat myData;
    //上
    if(IsPress_Down()){
      cursor_settingBool=cursor_settingBool+1;
      if(cursor_settingBool>boolCursorMax){
        cursor_settingBool=0;
      }
      
      while(IsPress_Down());
      setYourVal_Bool_preview();
    }
    //下
    if(IsPress_UP()){
      cursor_settingBool=cursor_settingBool-1;
      if(cursor_settingBool<0){
        cursor_settingBool=boolCursorMax;
      }
      //while(IsPress_Down());
      
      while(IsPress_UP());
      setYourVal_Bool_preview();
    }
    
    return cursor_settingBool;
}


*/

