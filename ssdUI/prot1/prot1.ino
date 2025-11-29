#include <EEPROM.h>
#include <stddef.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define RightPin A1
#define LeftPin  A0
#define OKPin    A3
#define BackPin  A2
#define SPKPin   6
 

const char* app1Name = "LED Power";
const char* app2Name = "App2";
const char* app3Name = "App3";
const char* app4Name = "Show all EEPROM data";

volatile int app=0;//割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
int cursor=0;
int HowManyApps=4;

int LEDBrightness=80;

//型を定義 EEPROMに書き込む変数を増やす場合はここに定義
struct DataFormat {
  char* ROM_teamName;
  int ROM_LED_output;
};

void setup() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 ERROR"));
    for(;;); 
}
  display.clearDisplay();
  
  //DataFormat myData = {"URAWA BEOLSAE (2025)",255,1,3.14};// ←初期値入れる場合
  DataFormat myData;//型を使ってEEPROM用の変数を作成
  EEPROM.put(0, myData);  // 先頭(アドレス0)から書き込む

  pinMode(RightPin, INPUT_PULLUP);
  pinMode(LeftPin, INPUT_PULLUP);
  pinMode(OKPin, INPUT_PULLUP);
  pinMode(BackPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), backToHome, RISING);
  randomSeed(analogRead(0));  
  startUpShow();
 // delay(900);
 // draw_HomeMenu();
}

void backToHome(){
    draw_HomeMenu();
    inHomeMenu_AppPreview();
    app=0;
}


void loop(){
  for(;;){
    if(app==0){homemenu();}
    else if(app==1){app1();}
    else if(app==2){app2();}
    else if(app==3){app3();}
    else if(app==4){app4();}
    Serial.print("cursor:");Serial.print(cursor);
    Serial.print(", app:");Serial.println(app);
  delay(30); 
  }
}

/*
文字サイズと使用するピクセル
TextSize 1 → 6×8
TextSize 2 → 12×16
TextSize 3 → 18×24
TextSize 4 → 24×32

*/
void printString_int(const char* ValName,int yourVal,int printIntX,int printIntY,int textSize){
  display.setCursor(printIntX,printIntY);
  display.setTextSize(textSize);
  display.print(ValName);
  display.print(":");
  display.print(yourVal);
  display.display();
}
void printString_float(const char* ValName,int yourVal,int printIntX,int printIntY,int textSize){
  display.setCursor(printIntX,printIntY);
  display.setTextSize(textSize);
  display.print(ValName);
  display.print(":");
  display.print(yourVal);
  display.display();
}
void printString_char(char* yourVal,int printIntX,int printIntY,int textSize){
  display.setCursor(printIntX,printIntY);
  display.setTextSize(textSize);
  display.print(yourVal);
  display.display();
}

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


//お飾り(起動時)
void startUpShow(){
  display.drawCircle(random(10, 118),random(10, 54),random(5, 80),1);
  display.drawCircle(random(10, 118),random(10, 54),random(5, 30),1);
  for(int i=0; i<20; i++){
    display.drawPixel(random(1,127),random(1, 63),1);
    delay(3);
  }
  display.setTextSize(2);
  display.setCursor(5,20);
  display.setTextColor(SSD1306_WHITE);
  display.print("Beolsae");
  display.display();
  //delay(1000);
}

void draw_HomeMenu(){
  display.clearDisplay();
  inHomeMenu_AppPreview();
}

//const int cursorCircleX,cursorCircleY,cursorCircleR = 10,10,10;
const int cursorCircleX = 3;
const int cursorCircleY = 3;
const int cursorCircleR = 3;
void inHomeMenu_AppPreview(){//HomeMenuに内包された絵画用関数(カーソル移動のたびに実行)
    display.clearDisplay();
    display.drawCircle(cursorCircleX                                        , cursorCircleY, cursorCircleR,1);//1
    display.drawCircle(cursorCircleX+cursorCircleR*(2*(  1  ))+3*(  1  ), cursorCircleY, cursorCircleR,1);//1
    display.drawCircle(cursorCircleX+cursorCircleR*(2*(  2  ))+3*(  2  ), cursorCircleY, cursorCircleR,1);//2
    display.drawCircle(cursorCircleX+cursorCircleR*(2*(  3  ))+3*(  3  ), cursorCircleY, cursorCircleR,1);//3
    display.drawCircle(cursorCircleX+cursorCircleR*(2*(  4  ))+3*(  4  ), cursorCircleY, cursorCircleR,1);//4
    display.setTextSize(2);
    display.setCursor(0,15);
    display.setTextColor(SSD1306_WHITE);
    display.print(cursor);
    display.print(":");
    //アプリのプレビュー表示
    switch(cursor){
      case 0:
        display.print("[app0]");
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
    }
    display.fillCircle(cursorCircleX+cursorCircleR*(2*(cursor))+3*(cursor), cursorCircleY, cursorCircleR,1);//2
    display.display();
}
void homemenu(){
  //カーソル移動
  if(IsPress_Right()){
    cursor=cursor+1;
    if(cursor>HowManyApps){
      cursor=0;
    }
    //inHomeMenu_AppPreview();
    while(IsPress_Right());
    inHomeMenu_AppPreview();
  }

  else if(IsPress_Left()){
    cursor=cursor-1;
    if(cursor<0){
      cursor=HowManyApps;
    }
    while(IsPress_Left());
    inHomeMenu_AppPreview();
  }

  //Okが押されたアプリを開く
  else if(IsPress_OK()){
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

     // default:
       // draw_HomeMenu();
    //    break;
    }
    
  }
}
void draw_app1(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print(app1Name);
  display.display();  
}
void draw_app2(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print(app2Name);
  display.display();
}
void draw_app3(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print(app3Name);
  display.display();
}
void draw_app4(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print(app4Name);
  display.display();
}


  const unsigned long longPressTime = 500; // 長押しとみなす時間（ミリ秒）
  int counter = 0;
  bool lastUPButtonState = LOW;
  bool lastDOWNButtonState = LOW;
  unsigned long buttonPressStart = 0;
  void app1(){//LEDの明るさ調整
    DataFormat myData;
    if(digitalRead(BackPin)==LOW){
      draw_HomeMenu();
      inHomeMenu_AppPreview();
      app=0;
    }
    else{


    //上
    bool UPbuttonState = digitalRead(RightPin) == LOW; // 押されているとき true
    if(IsPress_Right() && !lastUPButtonState){
      buttonPressStart = millis();
      counter++; // 単押しとして1加算
      LEDBrightness=LEDBrightness+1;
      if(LEDBrightness>255){
        LEDBrightness=0;
      }
      display.fillRect(0,9,128,64-9,0);
      display.setCursor(48,28);
      display.setTextSize(2);
      display.print(LEDBrightness);
      display.fillTriangle(59, 23, 64, 18, 79, 23, SSD1306_WHITE);
      display.drawTriangle(59, 47, 64, 52, 69, 47, SSD1306_WHITE);
      display.fillRect(0,64-8,128,8,0);
      printString_char("press OK to save",0,64-8,1);
    }
    if (UPbuttonState) {// ボタンが押され続けている場合
      unsigned long pressDuration = millis() - buttonPressStart;
      if (pressDuration >= longPressTime) {
        // 長押し判定：押している間毎ループ加算
        counter++;
        LEDBrightness=LEDBrightness+1;
      if(LEDBrightness>255){
        LEDBrightness=0;
      }
      display.fillRect(0,9,128,64-9,0);
      display.setCursor(48,28);
      display.setTextSize(2);
      display.print(LEDBrightness);
      display.fillTriangle(59, 23, 64, 18, 79, 23, SSD1306_WHITE);
      display.drawTriangle(59, 47, 64, 52, 69, 47, SSD1306_WHITE);
      display.fillRect(0,64-8,128,8,0);
      printString_char("press OK to save",0,64-8,1);
        //delay(30);
      }
      
    }
    lastUPButtonState = UPbuttonState;



    bool DOWNbuttonState = digitalRead(LeftPin) == LOW; // 押されているとき true
    if(IsPress_Left() && !lastDOWNButtonState){
      buttonPressStart = millis();
      counter++; // 単押しとして1加算
      LEDBrightness=LEDBrightness-1;
      if(LEDBrightness<0){
        LEDBrightness=255;
      }
      //while(IsPress_Left());
      display.fillRect(0,9,128,64-9,0);
      display.fillRect(0,9,128,64-9,0);
      display.setCursor(50,28);
      display.setTextSize(2);
      display.print(LEDBrightness);
      display.drawTriangle(59, 23, 64, 18, 79, 23, SSD1306_WHITE);
      display.fillTriangle(59, 47, 64, 52, 69, 47, SSD1306_WHITE);
      display.fillRect(0,64-8,128,8,0);
      printString_char("press OK to save",0,64-8,1);
    }
    if (DOWNbuttonState) {// ボタンが押され続けている場合
      unsigned long pressDuration = millis() - buttonPressStart;
      if (pressDuration >= longPressTime) {
        // 長押し判定：押している間毎ループ加算
        counter++;
        LEDBrightness=LEDBrightness-1;
      if(LEDBrightness<0){
        LEDBrightness=255;
      }
      display.fillRect(0,9,128,64-9,0);
      display.setCursor(48,28);
      display.setTextSize(2);
      display.print(LEDBrightness);
      display.drawTriangle(59, 23, 64, 18, 79, 23, SSD1306_WHITE);
      display.fillTriangle(59, 47, 64, 52, 69, 47, SSD1306_WHITE);
      display.fillRect(0,64-8,128,8,0);
      printString_char("press OK to save",0,64-8,1);
        //delay(30);
      }
      
    }
    lastDOWNButtonState = DOWNbuttonState;



    if(IsPress_OK()){
    //個別に書き込み
    myData.ROM_LED_output = LEDBrightness;
    EEPROM.put(offsetof(DataFormat,ROM_LED_output),myData.ROM_LED_output);
    display.fillRect(0,64-8,128,8,0);
    printString_char("saved!",70,64-8,1);
    }

    }
  }

  void app2(){
    if(digitalRead(BackPin)==LOW){
      draw_HomeMenu();
      inHomeMenu_AppPreview();
      app=0;
    }
    else{
      


    }
  }

void app3(){
  if(digitalRead(BackPin)==LOW){
    draw_HomeMenu();
    inHomeMenu_AppPreview();
    app=0;
  }
  else{
    


  }
}

void app4(){
  if(digitalRead(BackPin)==LOW){
    draw_HomeMenu();
    inHomeMenu_AppPreview();
    app=0;
  }
  else{
  DataFormat readData;
  EEPROM.get(0, readData);
  printString_char(readData.ROM_teamName,0,15,1);
  printString_int("LED",readData.ROM_LED_output,0,15+8,1);
  int eepromSize = EEPROM.length();  // EEPROM の総バイト数
  printString_int("EEPROM-length",eepromSize,0,15+8+8,1);
  }
}
/*
文字サイズと使用するピクセル
TextSize 1 → 6×8
TextSize 2 → 12×16
TextSize 3 → 18×24
TextSize 4 → 24×32
*/