//ホームメニューとその他グローバル関数
//FreeSansBoldOblique9pt7b

//デバッグ用--------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------














void backToHome(){
    draw_HomeMenu();
    inHomeMenu_AppPreview();
    app=0;
}
//以下システム用雑用関数---------------------------------------------------------------------------------------------------------------------------

//デバッグ用関数。削除可
void buttonTest_Serial(){
  Serial.print("R:");Serial.print(digitalRead(RightPin));
  Serial.print(", L:");Serial.print(digitalRead(LeftPin));
  Serial.print(", OK:");Serial.print(digitalRead(OKPin));
  Serial.print(", Back:");Serial.println(digitalRead(BackPin));
}

//ボタンが押されたかの判定
bool IsPress_Right(){
  return digitalRead(RightPin) == LOW;
}
bool IsPress_Left(){
  return digitalRead(LeftPin) == LOW;
}
bool IsPress_OK(){
  return digitalRead(OKPin) == LOW;
}
bool IsPress_Back(){
  return digitalRead(BackPin) == LOW;
}


//以下ホームメニュー関係----------------------------------------------------------------------------------------------------
//ホームメニューで選択中のアプリを表示する絵画関数
const int TriangleLength = 5;
const int cursorCircleR = 3;
void inHomeMenu_AppPreview(){//HomeMenuに内包された絵画用関数(カーソル移動のたびに実行)
    display.clearDisplay();
    display.drawCircle(cursorCircleR,cursorCircleR,cursorCircleR,1);//0
    display.drawCircle(cursorCircleR,cursorCircleR*3 + 2 ,cursorCircleR,1);//1
    display.drawCircle(cursorCircleR,cursorCircleR*5 + 2*2 ,cursorCircleR,1);//2
    display.drawCircle(cursorCircleR,cursorCircleR*7 + 2*3 ,cursorCircleR,1);//3
    display.drawCircle(cursorCircleR,cursorCircleR*9 + 2*4 ,cursorCircleR,1);//4
    display.drawCircle(cursorCircleR,cursorCircleR*11+ 2*5 ,cursorCircleR,1);//5
    display.drawLine(cursorCircleR*2+2,2,cursorCircleR*2+2,64-2,1);//縦線
    //アプリ名
    display.setFont(NULL);display.setTextSize(1);
    display.setCursor(0,64-8);
    display.print(cursor);
    display.setCursor(20,28);
    display.setFont(&FreeMono9pt7b);
    display.setTextColor(SSD1306_WHITE);
    //アプリのプレビュー表示
    switch(cursor){
      case 0:
        draw_app0();
        break;
      case 1:
        display.print(app1Name);
        break;
      case 2:
        display.print(app2Name);
        break;
      case 3:
        display.print(app3Name);
        break;
      case 4:
        display.print(app4Name);
        break;
      case 5:
        display.print(app5Name);
        break;
    }
    display.setFont(NULL);display.setTextSize(1);
    display.fillCircle(cursorCircleR,cursorCircleR + cursorCircleR*2*cursor + 2*cursor ,cursorCircleR,1);
    display.display();
}


void homemenu(){
  //カーソル移動
  if(IsPress_Left()){
    tone(SPKPin, 440, 20);
    cursor=cursor+1;
    if(cursor>HowManyApps){
      cursor=0;
    }
     myData.ROM_cursor = cursor;
     EEPROM.put(offsetof(DataFormat,ROM_cursor),myData.ROM_cursor);

    //inHomeMenu_AppPreview();
    //三角
    //display.drawTriangle(128-TriangleLength*2,20+TriangleLength,    127-TriangleLength,19,    126,20+TriangleLength,1);
    display.fillTriangle(128-TriangleLength*2,64-20-TriangleLength, 127-TriangleLength,64-19, 126,64-20-TriangleLength,1);
    display.display();
    while(IsPress_Left());
    inHomeMenu_AppPreview();
  }

  else if(IsPress_Right()){
    tone(SPKPin, 440, 20);
    cursor=cursor-1;
    if(cursor<0){
      cursor=HowManyApps;
    }
     myData.ROM_cursor = cursor;
     EEPROM.put(offsetof(DataFormat,ROM_cursor),myData.ROM_cursor);
    display.fillTriangle(128-TriangleLength*2,20+TriangleLength,    127-TriangleLength,19,    126,20+TriangleLength,1);
    //display.drawTriangle(128-TriangleLength*2,64-20-TriangleLength, 127-TriangleLength,64-19, 126,64-20-TriangleLength,1);
    display.display();
    while(IsPress_Right());
    inHomeMenu_AppPreview();
  }

  //Okが押されたアプリを開く
  else if(IsPress_OK()){
    if(cursor==0){
      tone(SPKPin,110,100);
    }
    else{
    tone(SPKPin, 550, 50);
    delay(80);
    tone(SPKPin, 750, 100);
    app=cursor;
    switch(cursor){
      case 1:
        draw_app1();
        break;
      case 2:
        draw_app2();
        break;
      case 3:
        app=3;
        draw_app3();
        break;
      case 4:
        draw_app4();
        break;
      case 5:
        draw_app5();
        break;

     // default:
       // draw_HomeMenu();
    //    break;
    }
    }
  }
    display.fillTriangle(128-TriangleLength*2,20+TriangleLength,    127-TriangleLength,19,    126,20+TriangleLength,0);
    display.fillTriangle(128-TriangleLength*2,64-20-TriangleLength, 127-TriangleLength,64-19, 126,64-20-TriangleLength,0);
    //display.drawTriangle(128-TriangleLength*2,20+TriangleLength,    127-TriangleLength,19,    126,20+TriangleLength,1);
    //display.drawTriangle(128-TriangleLength*2,64-20-TriangleLength, 127-TriangleLength,64-19, 126,64-20-TriangleLength,1);
    display.display();
}

//-----------------------------------------------------------------------------------------------------------------------------





//以下値調整アプリ用関数---------------------------------------------------------------------------------------------------------

const unsigned long longPressTime = 500; // 長押しとみなす時間（ミリ秒）
int counter = 0;
bool lastUPButtonState = LOW;
bool lastDOWNButtonState = LOW;
unsigned long buttonPressStart = 0;


int yourIntVal;
int setYourVal_int(int yourIntValMin,int yourIntValMax){//StructTypeには共通でmyDataを入れる
    DataFormat myData;
    //上
    bool UPbuttonState = digitalRead(RightPin) == LOW; // 押されているとき true
    if(IsPress_Right() && !lastUPButtonState){
      buttonPressStart = millis();
      counter++; // 単押しとして1加算
      yourIntVal=yourIntVal+1;
      if(yourIntVal>yourIntValMax){
        yourIntVal=yourIntValMin;
      }
      tone(SPKPin, 440, 20);
      display.fillRect(0,9,128,64-9,0);
      display.setCursor(52,35);
      display.setTextSize(1);
      display.setFont(&FreeSans9pt7b);
      display.print(yourIntVal);
      display.setFont(NULL);
      display.fillTriangle(59, 23, 64, 18, 69, 23, SSD1306_WHITE);
      display.drawTriangle(59, 46, 64, 51, 69, 46, SSD1306_WHITE);
      display.fillRect(0,64-8,128,8,0);
      printString_char("press OK to save",0,64-8);
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
      tone(SPKPin, 440, 20);
      display.fillRect(0,9,128,64-9,0);
      display.setCursor(52,35);
      display.setTextSize(1);
      display.setFont(&FreeSans9pt7b);
      display.print(yourIntVal);
      display.setFont(NULL);
      display.fillTriangle(61, 23, 64, 18, 67, 23, SSD1306_WHITE);
      display.drawTriangle(59, 46, 64, 51, 69, 46, SSD1306_WHITE);
      display.fillRect(0,64-8,128,8,0);
      printString_char("press OK to save",0,64-8);
        //delay(30);
      }
      
    }
    lastUPButtonState = UPbuttonState;


    //下
    bool DOWNbuttonState = digitalRead(LeftPin) == LOW; // 押されているとき true
    if(IsPress_Left() && !lastDOWNButtonState){
      buttonPressStart = millis();
      counter++; // 単押しとして1加算
      yourIntVal=yourIntVal-1;
      if(yourIntVal<yourIntValMin){
        yourIntVal=yourIntValMax;
      }
      //while(IsPress_Left());
      tone(SPKPin, 440, 20);
      display.fillRect(0,9,128,64-9,0);
      display.fillRect(0,9,128,64-9,0);
      display.setCursor(52,35);
      display.setTextSize(1);
      display.setFont(&FreeSans9pt7b);
      display.print(yourIntVal);
      display.setFont(NULL);
      display.drawTriangle(59, 23, 64, 18, 69, 23, SSD1306_WHITE);
      display.fillTriangle(59, 46, 64, 51, 69, 46, SSD1306_WHITE);
      display.fillRect(0,64-8,128,8,0);
      printString_char("press OK to save",0,64-8);
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
      tone(SPKPin, 440, 20);
      display.fillRect(0,9,128,64-9,0);
      display.setCursor(52,35);
      display.setTextSize(1);
      display.setFont(&FreeSans9pt7b);
      display.print(yourIntVal);
      display.setFont(NULL);
      display.drawTriangle(61, 23, 64, 18, 67, 23, SSD1306_WHITE);
      display.fillTriangle(61, 47, 64, 52, 67, 47, SSD1306_WHITE);
      display.fillRect(0,64-8,128,8,0);
      printString_char("press OK to save",0,64-8);
        //delay(30);
      }
      
    }
    lastDOWNButtonState = DOWNbuttonState;

    if(!IsPress_Left() && !IsPress_Right()){
      display.fillTriangle(59, 23, 64, 18, 69, 23, 0);
      display.fillTriangle(59, 46, 64, 51, 69, 46, 0);
      display.drawTriangle(59, 23, 64, 18, 69, 23, 1);
      display.drawTriangle(59, 46, 64, 51, 69, 46, 1);
      display.display();
    }
    return yourIntVal;
}


//以下float用
float yourFloatVal;
float setYourVal_float(float yourFloatValMin,float yourFloatValMax){//StructTypeには共通でmyDataを入れる
    DataFormat myData;
    //上
    bool UPbuttonState = digitalRead(RightPin) == LOW; // 押されているとき true
    if(IsPress_Right() && !lastUPButtonState){
      buttonPressStart = millis();
      counter++; // 単押しとして1加算
      yourFloatVal=yourFloatVal+1;
      if(yourFloatVal>yourFloatValMax){
        yourFloatVal=yourFloatValMin;
      }
      tone(SPKPin, 440, 20);
      display.fillRect(0,9,128,64-9,0);
      display.setCursor(49,28);
      display.setTextSize(2);
      display.print(yourFloatVal);
      display.fillTriangle(59, 23, 64, 18, 69, 23, SSD1306_WHITE);
      display.drawTriangle(59, 46, 64, 51, 69, 46, SSD1306_WHITE);
      display.fillRect(0,64-8,128,8,0);
      printString_char("press OK to save",0,64-8);
    }
    if (UPbuttonState) {// ボタンが押され続けている場合
      unsigned long pressDuration = millis() - buttonPressStart;
      if (pressDuration >= longPressTime) {
        // 長押し判定：押している間毎ループ加算
        counter++;
        yourFloatVal=yourFloatVal+1;
      if(yourFloatVal>yourFloatValMax){
        yourFloatVal=yourFloatValMin;
      }
      tone(SPKPin, 440, 20);
      display.fillRect(0,9,128,64-9,0);
      display.setCursor(49,28);
      display.setTextSize(2);
      display.print(yourFloatVal);
      display.fillTriangle(61, 23, 64, 18, 67, 23, SSD1306_WHITE);
      display.drawTriangle(59, 46, 64, 51, 69, 46, SSD1306_WHITE);
      display.fillRect(0,64-8,128,8,0);
      printString_char("press OK to save",0,64-8);
        //delay(30);
      }
      
    }
    lastUPButtonState = UPbuttonState;


    //下
    bool DOWNbuttonState = digitalRead(LeftPin) == LOW; // 押されているとき true
    if(IsPress_Left() && !lastDOWNButtonState){
      buttonPressStart = millis();
      counter++; // 単押しとして1加算
      yourFloatVal=yourFloatVal-1;
      if(yourFloatVal<yourFloatValMin){
        yourFloatVal=yourFloatValMax;
      }
      //while(IsPress_Left());
      tone(SPKPin, 440, 20);
      display.fillRect(0,9,128,64-9,0);
      display.fillRect(0,9,128,64-9,0);
      display.setCursor(49,28);
      display.setTextSize(2);
      display.print(yourFloatVal);
      display.drawTriangle(59, 23, 64, 18, 69, 23, SSD1306_WHITE);
      display.fillTriangle(59, 46, 64, 51, 69, 46, SSD1306_WHITE);
      display.fillRect(0,64-8,128,8,0);
      printString_char("press OK to save",0,64-8);
    }
    if (DOWNbuttonState) {// ボタンが押され続けている場合
      unsigned long pressDuration = millis() - buttonPressStart;
      if (pressDuration >= longPressTime) {
        // 長押し判定：押している間毎ループ加算
        counter++;
        yourFloatVal=yourFloatVal-1;
      if(yourFloatVal<yourFloatValMin){
        yourFloatVal=yourFloatValMax;
      }
      tone(SPKPin, 440, 20);
      display.fillRect(0,9,128,64-9,0);
      display.setCursor(49,28);
      display.setTextSize(2);
      display.print(yourFloatVal);
      display.drawTriangle(61, 23, 64, 18, 67, 23, SSD1306_WHITE);
      display.fillTriangle(61, 47, 64, 52, 67, 47, SSD1306_WHITE);
      display.fillRect(0,64-8,128,8,0);
      printString_char("press OK to save",0,64-8);
        //delay(30);
      }
      
    }
    lastDOWNButtonState = DOWNbuttonState;

    if(!IsPress_Left() && !IsPress_Right()){
      display.fillTriangle(59, 23, 64, 18, 69, 23, 0);
      display.fillTriangle(59, 46, 64, 51, 69, 46, 0);
      display.drawTriangle(59, 23, 64, 18, 69, 23, 1);
      display.drawTriangle(59, 46, 64, 51, 69, 46, 1);
      display.display();
    }
    return yourFloatVal;
}




//以下Bool用

// 1. 名前配列を定義
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
    if(IsPress_Left()){
      cursor_settingBool=cursor_settingBool+1;
      if(cursor_settingBool>boolCursorMax){
        cursor_settingBool=0;
      }
      
      while(IsPress_Left());
      setYourVal_Bool_preview();
    }
    //下
    if(IsPress_Right()){
      cursor_settingBool=cursor_settingBool-1;
      if(cursor_settingBool<0){
        cursor_settingBool=boolCursorMax;
      }
      //while(IsPress_Left());
      
      while(IsPress_Right());
      setYourVal_Bool_preview();
    }
    
    return cursor_settingBool;
}

