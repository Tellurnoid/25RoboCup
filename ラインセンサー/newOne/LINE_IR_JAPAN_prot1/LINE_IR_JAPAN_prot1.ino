#define S0 13
#define S1 12
#define S2 22
#define S3 23
#define LINE_COM1 35
#define LINE_COM2 34
#define IR_COM    32
#define LED       27
#define side_front  36
#define side_back   39
#define side_right  25
#define side_left   33
#define ball_sensor 15

const uint8_t ch_IR[16] = {
  8,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9
};
const uint8_t ch_LINE[32] = {
  11,12,13,14,15,10, 9, 8,
  31,30,29,28,24,25,26,27,
  16,17,18,19,20,21,22,23,
   3, 2, 1, 0, 4, 5, 6, 7
};

//ch対応なし、並べ替え前の仮格納用
uint16_t rawData_IR[16];
uint16_t rawData_LINE[32];
// 読み取り結果格納用(半時計回り)
uint16_t sensorValue_IR[16];
uint16_t sensorValue_LINE[32];

void setup(){
  Serial.begin(230400);
  initMUX();
}

void loop(){
  readMUX();
  printAllMUX();
  // readMUX_onlyLINE_debug();
}

void initMUX(){
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
}

void selectChannel_16(int ch) {
  ch &= 0x0F;  // 0〜15に制限
  digitalWrite(S0,  ch        & 0x01);
  digitalWrite(S1, (ch >> 1)  & 0x01);
  digitalWrite(S2, (ch >> 2)  & 0x01);
  digitalWrite(S3, (ch >> 3)  & 0x01);
}

void readMUX(){
  //読む
  for(uint8_t ch=0; ch<16; ch++){
    selectChannel_16(ch);
    // delayMicroseconds(10);
    //まず一つの配列、rawDataに流し込む

  analogRead(LINE_COM1);//捨て読み
  rawData_LINE[ch] = analogRead(LINE_COM1);
  delayMicroseconds(10);

  analogRead(LINE_COM2);//捨て読み
  rawData_LINE[16+ch] = analogRead(LINE_COM2);
  delayMicroseconds(10);

  analogRead(IR_COM);//捨て読み
  rawData_IR[ch] = analogRead(IR_COM);
  delayMicroseconds(10);
  }
  //整列させて代入
  for(uint8_t i=0; i<16; i++){
    sensorValue_IR[i]      = rawData_IR[ch_IR[i]];
    }
   for(uint8_t i = 0; i < 32; i++){
     sensorValue_LINE[i] = rawData_LINE[ch_LINE[i]];
   }
  }

void readMUX_onlyIR_debug(){
  //読む
  for(uint8_t ch=0; ch<16; ch++){
    selectChannel_16(ch);
    delayMicroseconds(10);
    //まず一つの配列、rawDataに流し込む
    sensorValue_IR[ch]      = analogRead(IR_COM);
  }
  //整列させて代入
      Serial.print("IR: ");
      for (uint8_t ch = 0; ch < 16; ch++) {
        Serial.print(",");
        Serial.print(sensorValue_IR[ch]/10);
      }
      Serial.println();
}
void readMUX_onlyLINE_debug(){
  //読む
  for(uint8_t ch=0; ch<32; ch++){
    selectChannel_16(ch);
    delayMicroseconds(10);
    //まず一つの配列、rawDataに流し込む
    sensorValue_LINE[ch]    = analogRead(LINE_COM1);

           Serial.print(",  LINE: ");
      for (uint8_t ch = 0; ch < 32; ch++) {
        Serial.print(",");
        Serial.print(sensorValue_LINE[ch]);
      }
      Serial.println();
  }
}

  void printAllMUX(){
      Serial.print("IR: ");
      for (uint8_t ch = 0; ch < 16; ch++) {
        Serial.print(",");
        Serial.print(sensorValue_IR[ch]/10);
      }
      Serial.print(",  LINE: ");
      for (uint8_t ch = 0; ch < 32; ch++) {
        Serial.print(",");
        Serial.print(sensorValue_LINE[ch]/10);
      }
      Serial.println();
  }
  void printIR(){
      Serial.print("IR: ");
      for (uint8_t ch = 0; ch < 16; ch++) {
        Serial.print(",");
        Serial.print(sensorValue_IR[ch]);
      }
      Serial.println();
  }

  void printLINE(){
      Serial.print(",  LINE: ");
      for (uint8_t ch = 0; ch < 32; ch++) {
        Serial.print(",");
        Serial.print(sensorValue_LINE[ch]);
      }
      Serial.println();
  }