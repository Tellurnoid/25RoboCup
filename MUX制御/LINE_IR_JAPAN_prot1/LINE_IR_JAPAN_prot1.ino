
#define S0 13
#define S1 12
#define S2 11
#define S3 10
#define COM1 A0

int sensorValue[2][16];// 読み取り結果を格納

void setup(){
    Serial.begin(115200);
}

void loop(){
    readMUX_16();
//printAllMUX_16();
    Serial.println(sensorValue[0][0]);
    delay(10);
}

void initMUX_16(){
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
}

void selectChannel_16(int ch) {
  digitalWrite(S0, ch & 0x01);
  digitalWrite(S1, (ch >> 1) & 0x01);
  digitalWrite(S2, (ch >> 2) & 0x01);
  digitalWrite(S3, (ch >> 3) & 0x01);
}

void readMUX_16(){
//    delayMicroseconds(5); // 切り替え安定待ち
    for (int ch = 0; ch < 16; ch++) {//読み込む
      selectChannel_16(ch);
      delayMicroseconds(5); // チャンネル安定待ち
      sensorValue[0][ch] = analogRead(COM1);
    }  
  }

  void printAllMUX_16(){
      for (int ch = 0; ch < 16; ch++) {
        Serial.print(",");
        Serial.print(sensorValue[0][ch]/100);
      }
      Serial.println();
}