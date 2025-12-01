//FreeSansBoldOblique9pt7b
//以下画面に文字、変数を表示する関数----------------------------------------------------------------------------------
void printString_int(const char* ValName, int yourVal, int printIntX, int printIntY) {
  display.setCursor(printIntX, printIntY);
  display.print(ValName);
  display.print(":");
  display.print(yourVal);
  display.display();
}

void printString_float(const char* ValName, int yourVal, int printIntX, int printIntY) {
  display.setCursor(printIntX, printIntY);
  display.print(ValName);
  display.print(":");
  display.print(yourVal);
  display.display();
}
void printString_char(char* yourVal, int printIntX, int printIntY) {
  display.setCursor(printIntX, printIntY);
  display.print(yourVal);
  display.display();
}



//起動時の演出

void startUpShow() {
  display.drawCircle(random(10, 118), random(10, 54), random(5, 80), 1);
  display.drawCircle(random(10, 118), random(10, 54), random(5, 30), 1);
  for (int i = 0; i < 20; i++) {
    display.drawPixel(random(1, 127), random(1, 63), 1);
    delay(3);
  }
  display.setFont(&FreeSansBoldOblique9pt7b);
  display.setCursor(5, 20);
  display.setTextColor(SSD1306_WHITE);
  display.print("Beolsae");
  display.display();
  display.setFont(NULL);
  display.setTextSize(1);
  //delay(1000);
}


//以下アプリを開いたときに一度実行する関数---------------------------------------------------------------------------
void draw_HomeMenu() {
  DataFormat readData;
  EEPROM.get(0, readData);
  cursor=readData.ROM_cursor;
  display.clearDisplay();
  //お飾り。起動画面と同じ
  //display.drawCircle(random(10, 118),random(10, 54),random(5, 80),1);
  //display.drawCircle(random(10, 118),random(10, 54),random(5, 30),1);
  for (int i = 0; i < 20; i++) {
    display.drawPixel(random(1, 127), random(1, 63), 1);
    delay(3);
  }
  inHomeMenu_AppPreview();
}

void draw_app0() {
  DataFormat readData;
  EEPROM.get(0, readData);
  display.setFont(&FreeMono9pt7b);
  printString_char("EEPROM", 20, 10);
  display.setFont(NULL);
  display.setTextSize(1);
  printString_int("LED", readData.ROM_LED_output, 20, 15);
  printString_char("Bools:{",20,15+8);
  if(BoolVals[0]==1){printString_char("TRUE",27,15+8*2);}else{printString_char("FALSE",27,15+8*2);}
  if(BoolVals[1]==2){printString_char("TRUE",27,15+8*3);}else{printString_char("FALSE",27,15+8*3);}
  if(BoolVals[2]==3){printString_char("TRUE",27,15+8*4);}else{printString_char("FALSE",27,15+8*4);}
  printString_char("}",20,15+8*5);
}

void draw_app1() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setFont(NULL);
  display.setTextSize(1);
  display.print(app1Name);
  display.display();
  //編集する変数に書き換えてください。---------
  DataFormat readData;
  EEPROM.get(0, readData);
  LEDBrightness = readData.ROM_LED_output;
  //-------------------------------------
  display.fillRect(0, 9, 128, 64 - 9, 0);
  display.setCursor(52, 35);
  display.setTextSize(1);
  display.setFont(&FreeSans9pt7b);
  display.print(readData.ROM_LED_output);
  display.setFont(NULL);
  printString_char("press OK to save", 0, 64 - 8);
  if (IsPress_OK())
    while (IsPress_OK());
}
void draw_app2() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setFont(NULL);
  display.setTextSize(1);
  display.print(app2Name);
  //-----------------------------------------------EEPROMに書き込む場合ここに変数読み込みの関数を書く
  DataFormat readData;
  EEPROM.get(0, readData);
  BoolVals[0] = readData.ROM_BoolVal1;
  BoolVals[1] = readData.ROM_BoolVal2;
  BoolVals[2] = readData.ROM_BoolVal3;
     for(int i=0; i<boolCursorMax; i++){
      printString_char(BoolValsNames[i],15,8+i*8);
       if(BoolVals[i]==0){
        printString_char("TRUE",128-6*5,8+i*8);
       }
       else{
        printString_char("FALSE",128-6*5,8+i*8);
       }
      }
  display.display();
}
void draw_app3() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setFont(NULL);
  display.setTextSize(1);
  display.print(app3Name);
  display.display();
}
void draw_app4() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setFont(NULL);
  display.setTextSize(1);
  display.print(app4Name);
  display.display();
}
void draw_app5() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setFont(NULL);
  display.setTextSize(1);
  display.print(app5Name);
  display.display();
}
//-----------------------------------------------------------------------------------------------