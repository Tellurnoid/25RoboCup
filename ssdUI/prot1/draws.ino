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
  printString_char(readData.ROM_teamName, 20, 19);
  printString_int("LED", readData.ROM_LED_output, 20, 19 + 8);
  int eepromSize = EEPROM.length();  // EEPROM の総バイト数
  printString_int("EEPROM-length", eepromSize, 20, 19 + 8 + 8);
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
    while (IsPress_OK())
      ;
}
void draw_app2() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setFont(NULL);
  display.setTextSize(1);
  display.print(app2Name);
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