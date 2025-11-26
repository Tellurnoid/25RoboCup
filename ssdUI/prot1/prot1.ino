#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <misakiUTF16.h> //日本語
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define RightPin 10
#define LeftPin  9
#define OKPin    11
#define BackPin  12
 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
uint8_t font[8];                           // フォント格納バッファ

void setup() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 ERROR"));
    for(;;); 
}
  randomSeed(analogRead(0));  
  display.clearDisplay();
  //drawJa("日本語表示",0,32-8,2);
  pinMode(RightPin, INPUT_PULLUP);
  pinMode(LeftPin, INPUT_PULLUP);
  pinMode(OKPin, INPUT_PULLUP);
  pinMode(BackPin, INPUT_PULLUP);
  startUpShow();
  draw_HomeMenu();
}

int app=0;
int cursor=0;
int HowManyApps=4;
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


//以下日本語絵画用関数
//-----------------------------------------------------------------------------------
// ビットマップの拡大描画
void drawBitmapEx(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h,
                  uint16_t color,uint16_t bgcolor, uint16_t ex) {
  int16_t i, j,b=(w+7)/8;
  for( j = 0; j < h; j++) {
    for(i = 0; i < w; i++ ) { 
      if(*(bitmap + j*b + i / 8) & (128 >> (i & 7))) {
        // ドットあり
        if (ex == 1)  display.drawPixel(x+i, y+j, color); //1倍
        else          display.fillRect(x + i * ex, y + j * ex, ex, ex, color); // ex倍
      } else {
        // ドットなし
        if (ex == 1)  display.drawPixel(x+i, y+j, bgcolor);
        else          display.fillRect(x + i * ex, y + j * ex, ex, ex, bgcolor);
       }
     }
   }
}
void drawJa( char *str,uint16_t x,uint16_t y,int16_t ex){//"文字",x,y,サイズ(x,yはなん文字分か)
  char *ptr = str;
  while(*ptr) {                         // 文字列分ループ
     ptr = getFontData(font, ptr,true); // 1文字分のフォント取得
     if (!ptr)
        break;                          // エラーの場合は終
     drawBitmapEx(x,y,font,8,8,1,0,ex); // フレームバッファにフォント書き込み 
     if (128-8*ex > x) {                // 表示位置更新
       x+=8*ex;
     } else {
       x=0;
       y+=8*ex;
     }
  }

}
//-----------------------------------------------------------------------------------

//お飾り
void startUpShow(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  //display.fillRect(0,0,128,64,0);
  display.display();
}

void draw_HomeMenu(){
  display.clearDisplay();
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
        display.print("[app1]");
        break;
      case 2:
        display.print("[app2]");
        break;
      case 3:
        display.print("[app3]");
        break;
      case 4:
        display.print("[app4]");
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
  }

  else if(IsPress_Left()){
    cursor=cursor-1;
    if(cursor<0){
      cursor=HowManyApps;
    }
    while(IsPress_Left());
  }
  
  //Okが押されたアプリを開く
  else if(IsPress_OK()){
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
    app=cursor;
  }
  inHomeMenu_AppPreview();
}
void draw_app1(){}
void draw_app2(){}
void draw_app3(){}
void draw_app4(){}
void app1(){

}
void app2(){}
void app3(){}
void app4(){}