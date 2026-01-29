//RP2040用のソフトウェアシリアル試験用コード
//13にLED,起動時に点灯、'2'を受けたら消灯
//相手には'1'を送り続けています

#include <SoftwareSerial.h>
SoftwareSerial mySerial(14, 15); // RX, TX（ソフトウェアシリアル）
//ソフトウェアシリアルは全ピンで可能
//ただし0,1 , 8,9はハードウェアシリアルもある

void setup() {
  Serial .begin(9600); 
  mySerial.begin(9600);// ソフトウェアシリアル
  pinMode(13, OUTPUT);  
  digitalWrite(13,HIGH);

//ハードウェアシリアルあと2つ(動作確認済)↓
//Serial1 .begin(9600);   // UART0初期化 TX:GP0 / RX:GP1
//  Serial2 .begin(9600);   // UART1初期化 TX:GP8 / RX:GP9
}

char data = 0;
void loop() {
  if(mySerial.available()){
      data = mySerial.read();
      if(data == '2'){      
        digitalWrite(13,LOW);
        delay(500);
      }  
      else if(data == '0'){digitalWrite(13,HIGH);}
  }
  mySerial.println('1'); // ソフトウェアシリアル出力(相手のマイコンへ)
  delay(50);
}