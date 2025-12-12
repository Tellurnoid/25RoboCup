//FreeSansBoldOblique9pt7b
//以下画面に文字、変数を表示する関数----------------------------------------------------------------------------------
template <typename T>
void printString(const char* ValName, T yourVal, int printX, int printY) {
display.setCursor(printX, printY);
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

  //delay(1000);
}


//以下アプリを開いたときに一度実行する関数---------------------------------------------------------------------------
void draw_HomeMenu() {
  DataFormat readData;
  EEPROM.get(0, readData);
  cursor=readData.ROM_cursor;
  display.clearDisplay();
  inHomeMenu_AppPreview();
}

void draw_app0() {
  DataFormat readData;
  EEPROM.get(0, readData);
  display.setFont(&FreeMono9pt7b);
  printString_char("EEPROM", 20, 10);
  display.setFont(NULL);
  display.setTextSize(1);
  printString("LED", readData.ROM_LED_output, 20, 15);
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

//int costom1_nums[]={0,0,0,0,0};//systems.inoで定義済み
//const int FontDistX=4;//フォント使用によるドットずれの補正。systems.inoで定義済み
//const int FontDistY=6;
int costom1_nums[]={0,0,0,0,0};
//int costom1_nums[]={0,0,0,0,0};
int cursor_BigInt=0;
const int FontDistX=4;//フォント使用によるドットずれの補正
const int FontDistY=6;
long sum_bigInt;

void draw_app4(int costoms) {
  DataFormat readData;
  EEPROM.get(0, readData);
  sum_bigInt = readData.ROM_costom1;
  costom1_nums[0] = sum_bigInt / 10000 % 10;
  costom1_nums[1] = sum_bigInt / 1000  % 10;
  costom1_nums[2] = sum_bigInt / 100   % 10;
  costom1_nums[3] = sum_bigInt / 10    % 10;
  costom1_nums[4] = sum_bigInt % 10;

  cursor_BigInt=0;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setFont(NULL);
  display.setTextSize(1);
  display.print("costom");
  display.print(costoms);
      display.fillRect(0,25-FontDistY,128,64-25-FontDistY,0);
      //数字と三角のポインター
      display.setCursor(36,35);//左端x=37、右端x=37+30=67
      display.setTextSize(1);
      display.setFont(&FreeSans9pt7b);
      for(int i=0; i<5; i++){
        display.drawTriangle(36+10*i-2+FontDistX,35-5-FontDistY, 36+10*i+FontDistX,35-5-5-FontDistY, 36+10*i+2+FontDistX,35-5-FontDistY ,1);
        display.print(costom1_nums[i]);
        display.drawTriangle(36+10*i-2+FontDistX,35+11+7-FontDistY, 36+10*i+FontDistX,35+11+5+7-FontDistY, 36+10*i+2+FontDistX,35+11+7-FontDistY ,1);
      }
      //ガイド
      display.setFont(NULL);
      display.setCursor(6,32);
      display.print("< OK");
      display.setCursor(92,32);
      display.print("Back >");
      display.display();

}
void draw_app5() {

}
//-----------------------------------------------------------------------------------------------