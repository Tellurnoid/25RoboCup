//いつものハードウェアシリアル(ESP32用)
//13にLED,起動時に点灯、'1'を受けたら消灯
//相手には'2'を送り続けています
void setup(){
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 27,26);  //TX, RX // to IR_rp2040
  Serial.println("ready!");
}
char data = 0;
void loop() {
  if(Serial1.available()){
      data = Serial1.read();
      if(data == '1'){       //相手のマイコンから1を受信したら消灯
        digitalWrite(13,LOW);
        Serial.println("got1");
        delay(500);
      }  
      else if(data == '0'){digitalWrite(13,HIGH);Serial.println("got0");}
  }
  Serial1.print('2'); // ソフトウェアシリアル出力(相手のマイコンへ)
  delay(10);
}