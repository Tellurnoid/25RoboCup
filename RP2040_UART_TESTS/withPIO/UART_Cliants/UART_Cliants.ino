//いつものハードウェアシリアル
//13にLED,起動時に点灯、'1'を受けたら消灯
//相手には'2'を送り続けています
void setup(){
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  Serial.begin(115200);
  Serial.println("ready!");
}
char data = 0;
void loop() {
  if(Serial.available()){
      data = Serial.read();
      if(data == '1'){       //相手のマイコンから1を受信したら消灯
        digitalWrite(13,LOW);
        delay(500);
      }  
      else if(data == '0'){digitalWrite(13,HIGH);}
  }
  Serial.println('2'); // ソフトウェアシリアル出力(相手のマイコンへ)
  delay(50);
}