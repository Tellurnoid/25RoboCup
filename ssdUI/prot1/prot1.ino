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
 

const char* app1Name = "App1";
const char* app2Name = "App2";
const char* app3Name = "App3";
const char* app4Name = "App4";

volatile int app=0;//割り込みでいつでもapp=0(ホーム)に戻るためvolatileをつける
int cursor=0;
int HowManyApps=4;



void setup() {
  Serial.begin(115200);
  Serial.println("Hello");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 ERROR"));
    for(;;); 
}
  display.clearDisplay();

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
    display.setCursor(5,20);
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
void app1(){
  if(digitalRead(BackPin)==LOW){
    draw_HomeMenu();
    inHomeMenu_AppPreview();
    app=0;
  }
  else{
  //ここに記述

    for(int i=0; i<100; i++){
      display.clearDisplay();
      printString_int("i",i,10,32,2);
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
    


  }
}