#include <Arduino.h>


#include "BluetoothSerial.h"
#define USE_NAME  // Comment this to use MAC address instead of a slaveName
BluetoothSerial SerialBT;
#ifdef USE_NAME
String slaveName = "ESP32-BT-Slave";  // Change this to reflect the real name of your slave BT device
#else
String MACadd = "AA:BB:CC:11:22:33";                        // This only for printing
uint8_t address[6] = {0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33};  // Change this to reflect real MAC address of your slave BT device
#endif
String myName = "ESP32-BT-Master";



HardwareSerial &mySerial = Serial2; // UART2を使用する場合
int motorSpeed= 120;
#define INA1 33
#define INA2 32

#define INB1 26
#define INB2 25

#define INC1 27
#define INC2 14

#define IND1 13//5
#define IND2 4//12
void straight(){
  ledcWrite(INA1,motorSpeed);
  ledcWrite(INA2,0);
  ledcWrite(INB1,motorSpeed);
  ledcWrite(INB2,0);
  ledcWrite(INC1,motorSpeed);
  ledcWrite(INC2,0);
  ledcWrite(IND1,motorSpeed);
  ledcWrite(IND2,0);
}
void back(){
  ledcWrite(INA1, 0);
  ledcWrite(INA2,motorSpeed);
  ledcWrite(INB1,0);
  ledcWrite(INB2,motorSpeed);
  ledcWrite(INC1,0);
  ledcWrite(INC2,motorSpeed);
  ledcWrite(IND1,0);
  ledcWrite(IND2,motorSpeed);
}
void slideRight(){
  ledcWrite(INA1,0);
  ledcWrite(INA2,motorSpeed);
  ledcWrite(INB1,motorSpeed);
  ledcWrite(INB2,0);
  ledcWrite(INC1,0);
  ledcWrite(INC2,motorSpeed);
  ledcWrite(IND1,motorSpeed);
  ledcWrite(IND2,0);
}
void slideLeft(){
  ledcWrite(INA1, motorSpeed);
  ledcWrite(INA2,0);
  ledcWrite(INB1,0);
  ledcWrite(INB2,motorSpeed);
  ledcWrite(INC1,motorSpeed);
  ledcWrite(INC2, 0);
  ledcWrite(IND1,0);
  ledcWrite(IND2,motorSpeed);
}
void turnRight(){
  ledcWrite(INA1,0);
  ledcWrite(INA2,motorSpeed);
  ledcWrite(INB1,motorSpeed);
  ledcWrite(INB2,0);
  ledcWrite(INC1,motorSpeed);
  ledcWrite(INC2, 0);
  ledcWrite(IND1,0);
  ledcWrite(IND2,motorSpeed);
}
void turnLeft(){
  ledcWrite(INA1,motorSpeed);
  ledcWrite(INA2,0);
  ledcWrite(INB1,0);
  ledcWrite(INB2,motorSpeed);
  ledcWrite(INC1,0);
  ledcWrite(INC2,motorSpeed);
  ledcWrite(IND1,motorSpeed);
  ledcWrite(IND2,0);
}

void slideRightStraight(){
  ledcWrite(INA1,0);
  ledcWrite(INA2,0);
  ledcWrite(INB1,motorSpeed+150);
  ledcWrite(INB2,0);
  ledcWrite(INC1,0);
  ledcWrite(INC2,0);
  ledcWrite(IND1,motorSpeed+150);
  ledcWrite(IND2,0);
}
void slideLeftBack(){
  ledcWrite(INA1, 0);
  ledcWrite(INA2,0);
  ledcWrite(INB1,0);
  ledcWrite(INB2,motorSpeed+150);
  ledcWrite(INC1,0);
  ledcWrite(INC2,0);
  ledcWrite(IND1,0);
  ledcWrite(IND2,motorSpeed+150);
}
void slideLeftStraight(){
  ledcWrite(INA1,motorSpeed+150);
  ledcWrite(INA2,0);
  ledcWrite(INB1,0);
  ledcWrite(INB2,0);
  ledcWrite(INC1,motorSpeed+150);
  ledcWrite(INC2,0);
  ledcWrite(IND1,0);
  ledcWrite(IND2,0);
}
void slideRightBack(){
  ledcWrite(INA1, 0);
  ledcWrite(INA2,motorSpeed+150);
  ledcWrite(INB1,0);
  ledcWrite(INB2,0);
  ledcWrite(INC1,0);
  ledcWrite(INC2,motorSpeed+150);
  ledcWrite(IND1,0);
  ledcWrite(IND2,0);
}
void stop(){
  ledcWrite(INA1,0);
  ledcWrite(INA2,0);
  ledcWrite(INB1,0);
  ledcWrite(INB2,0);
  ledcWrite(INC1,0);
  ledcWrite(INC2,0);
  ledcWrite(IND1,0);
  ledcWrite(IND2,0);
}
const int freq = 550;       // 周波数 Hz
const int resolution = 8;    // 8ビット解像度（0〜motorSpeed）

void setup() {
  Serial.begin(115200);      // デバッグ用
  SerialBT.begin(myName, true);
  mySerial.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17 など適宜設定
  ledcAttach(INA1, freq, resolution);   
  ledcAttach(INA2, freq, resolution);
  ledcAttach(INB1, freq, resolution);   
  ledcAttach(INB2, freq, resolution);
  ledcAttach(INC1, freq, resolution);   
  ledcAttach(INC2, freq, resolution);
  ledcAttach(IND1, freq, resolution);   
  ledcAttach(IND2, freq, resolution);
   stop();

   ledcWrite(INB1,motorSpeed);
   ledcWrite(INB2,0);
   ledcWrite(INC1,motorSpeed);
   ledcWrite(INC2,0);
   delay(2000);
}
int Power;
int guess;//0:前を見る 1:左にあるかも 2:右にあるかも
void loop() {
  // データが6バイト以上あればブロブ情報を読む
  if (mySerial.available() >= 2) {
    // まず2バイト読み込む
    uint8_t buf[6];
    mySerial.readBytes(buf, 2);
    int first = (buf[0] << 8) | buf[1];

      if (first == 0xFFFF) {// ブロブなし
        Power=90;
        if(guess==0){stop();}
        else if(guess==1){turnLeft();delay(200);guess=0;}
        else{turnRight();delay(500);guess=0;}
        Serial.println("No blob detected");
      } 
      else 
      {
      // ブロブあり、残り4バイトも読む
        mySerial.readBytes(buf + 2, 4);
        int cx     = first;
        int cy     = (buf[2] << 8) | buf[3];
        int pixels = (buf[4] << 8) | buf[5];
        int paraCx = 160-cx;
        int paraMotor1,paraMotor2,paraMotor3,paraMotor4;
      
        if(pixels > 8000 ){
          Power = 0;
        }
        else
        {
          Power = motorSpeed - (pixels-100)*motorSpeed/9900; 
        }
       // デバッグ出力
        Serial.print("cx: "); Serial.print(cx);
        Serial.print(", cy: "); Serial.print(cy);
        Serial.print(", pixels: "); Serial.print(pixels);
        Serial.print(", Power: "); Serial.print(Power);
        Serial.print(", guess: "); Serial.print(guess);
        
       // if(pixels<1000){
        if(abs(paraCx)<25){
          straight();
          guess=0;
        }
        //else if(cx < 20){guess=1;}
        //else if(cx > 295){guess=2;}
        else if(paraCx < 0){
          guess=2;
          paraMotor1 = Power+(Power*paraCx/160);
          paraMotor2 = -1*Power*paraCx/160;
          ledcWrite(INA1,paraMotor1);
          ledcWrite(INA2,paraMotor2);
          ledcWrite(INB1,Power);
          ledcWrite(INB2,0);
          ledcWrite(INC1,paraMotor1);
          ledcWrite(INC2,paraMotor2);
          ledcWrite(IND1,Power);
          ledcWrite(IND2,0);
        }
        else if(paraCx > 0){
          guess=1;
          //paraMotor1 = Power*(1-paraCx/160);
          //paraMotor2 = Power-Power*(1-paraCx/160);
          paraMotor3 = -1*Power*paraCx/160;
          paraMotor4 = Power+(Power*paraCx/160);
          ledcWrite(INA1,Power);
          ledcWrite(INA2,0);
          ledcWrite(INB1,paraMotor3);
          ledcWrite(INB2,paraMotor4);
          ledcWrite(INC1,Power);
          ledcWrite(INC2,0);
          ledcWrite(IND1,paraMotor3);
          ledcWrite(IND2,paraMotor4);      
        }
        Serial.print(", paraCx:");Serial.print(paraCx);
        Serial.print(",paraMotor1:");Serial.print(paraMotor1);
        Serial.print(",paraMotor2:");Serial.println(paraMotor2);


        SerialBT.print("cx: "); SerialBT.print(cx);
        SerialBT.print(", area: "); SerialBT.print(pixels);
        SerialBT.print(", Power: "); SerialBT.println(Power);;
      }
/*    
    //青未検出
    if(cx > 320){
      stop();
    }
    //右に物体あり
    else if( (cx-160) > 90){
      turnRight();
    }
    else if( (cx-160) > 30){
      slideRight();
    }

    //左に物体あり
    else if( (cx-160) < -90){
      turnLeft();
    }
    else if( (cx-160) < 30){
      slideLeft();
    }
    else{
      straight();
    }
*/
  }
}
