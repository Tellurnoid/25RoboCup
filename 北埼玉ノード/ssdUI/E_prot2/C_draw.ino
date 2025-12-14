//#include <ssdUI_vals.h>
//以下画面に文字、変数を表示する関数----------------------------------------------------------------------------------
template <typename T>
void printString(const char* ValName, T yourVal, int printX, int printY) {
oled.setCursor(printX, printY);
oled.print(ValName);
oled.print(":");
oled.print(yourVal);
}

void printString_char(char* yourVal, int printIntX, int printIntY) {
  oled.setCursor(printIntX, printIntY);
  oled.print(yourVal);
}

void startUpShow(){
  oled.clear();
  oled.setCursor(0,0);
  oled.print("Beolsae");
}

void draw_homemenu(){}

void draw_setIntVal(){}

void draw_setFloatVal(){}