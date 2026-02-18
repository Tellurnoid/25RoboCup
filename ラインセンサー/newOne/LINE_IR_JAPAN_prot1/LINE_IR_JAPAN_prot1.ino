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
  32,31,30,29,25,26,27,28,
  17,18,19,20,21,22,23,24,
   3, 2, 1, 0, 4, 5, 6, 7
};

//ch対応なし、並べ替え前の仮格納用
uint16_t rawData_IR[16];
uint16_t rawData_LINE[32];
// 読み取り結果格納用(半時計回り)
uint16_t sensorValue_IR[16];
uint16_t sensorValue_LINE[32];

void setup(){
  initMUX();
}

void loop(){
    readMUX();
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
    delayMicroseconds(5);
    //まず一つの配列、rawDataに流し込む
    rawData_IR[ch]      = analogRead(IR_COM);
    rawData_LINE[ch]    = analogRead(LINE_COM1);
    rawData_LINE[16+ch] = analogRead(LINE_COM2);
  }
  //整列させて代入
  for(uint8_t i=0; i<16; i++){
    sensorValue_IR[i]      = rawData_IR[ch_IR[i]];
    sensorValue_LINE[i]    = rawData_LINE[ch_LINE[i]];
    sensorValue_LINE[16+i] = rawData_LINE[16+ch_LINE[16+i]];
  }
}

  void printAllMUX(){
      Serial.print("IR: ");
      for (uint8_t ch = 0; ch < 16; ch++) {
        Serial.print(",");
        Serial.print(sensorValue_IR[ch]);
      }
      Serial.print(",  LINE: ");
      for (uint8_t ch = 0; ch < 32; ch++) {
        Serial.print(",");
        Serial.print(sensorValue_LINE[ch]);
      }
      Serial.println();
  }