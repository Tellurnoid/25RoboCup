#include <U8x8lib.h>
#include <EEPROM.h>
#include "A_system.h"
#include "B_UART.h"
#include "C_draw.h"
#include "D_app.h"


// U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);
// UIsystem UIsystem;//A_system.hの中身
// draw draw;
// app app;

//超音波センサー
const int NUM_SENSOR = 4;
const int pin[NUM_SENSOR] = {5, 4, 3, 2};
//int16_t dis[NUM_SENSOR];      // ESP32へ送る距離（cm）


/*
メモ
ホーム
1:back to game (決定を押すと超音波センサーのUART送信開始)
2:Serial monitor
3:vals    intとfloatで、それぞれでもう一度分岐する
4:line sensor  キャリブレーション、全体調整、個別調整
5:bools     カメラ、超音波など一部機能の無効化

*/
int16_t readUltrasonic1Pin(uint8_t pin) {
  // --- 出力モードでトリガ発射 ---
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);

  // --- 入力モードに切り替えて Echo 待ち ---
  pinMode(pin, INPUT);

  // ✅ タイムアウト付き pulseIn（最大10ms ≒ 170cm）
  long t = pulseIn(pin, HIGH, 10000);

  // ✅ 反射なしは -1 を返す
  if (t == 0) return -1;

  // ✅ 距離へ変換（mm）
  return (int16_t)(t / 5.8);
}




bool IsOnGame = true; //trueだと超音波センサー,falseでUI起動
void setup(){
  uisystem.initButtons();
  if(uisystem.IsPress_UP() && uisystem.IsPress_Down()){
    IsOnGame = false;
    uisystem.decidedSound();
    drawObj.startupShow();
    EEPROM.get(0, myData);
    cursor = myData.ROM_cursor;
  }
  u8x8.begin();
  u8x8.setPowerSave(0);  // 表示ON
  u8x8.setFlipMode(0);
  //EEPROM
  //DataFormat readData;
  if (IsOnGame == false && !(uisystem.IsPress_OK() || uisystem.IsPress_Back() || uisystem.IsPress_UP() || uisystem.IsPress_Down()) ) {
    while (!(uisystem.IsPress_OK() || uisystem.IsPress_Back() || uisystem.IsPress_UP() || uisystem.IsPress_Down()));
  }
}

void loop(){
  if(IsOnGame == true){
    //超音波センサーの内容
    for (int i = 0; i < NUM_SENSOR; i++) {
      dis[i] = readUltrasonic1Pin(pin[i]);
    }
    UART_USsensor();
    u8x8.clear();
    u8x8.setCursor(0,0);
    u8x8.setFont(u8x8_font_8x13B_1x2_f);
    u8x8.print("A:");u8x8.print(dis[0]);u8x8.println("cm");
    u8x8.print("B:");u8x8.print(dis[1]);u8x8.println("cm");
    u8x8.print("C:");u8x8.print(dis[2]);u8x8.println("cm");
    u8x8.print("D:");u8x8.print(dis[3]);u8x8.println("cm");

    // Serial.print("A:");Serial.print(dis[0]);Serial.print("cm");
    // Serial.print("B:");Serial.print(dis[1]);Serial.print("cm");
    // Serial.print("C:");Serial.print(dis[2]);Serial.print("cm");
    // Serial.print("D:");Serial.print(dis[3]);Serial.println("cm");
    delay(10);  // 100Hz（これ以上速くするとセンサが不安定になる）

  
  }
  else{
    if(appState==0){
      app.homeMenu();
    }
    else if(appState==1){
      app.LEDBrightness();
    }
    else if(appState==2){
      app.SerialMonitor();
    }
  }
}
