#include <Arduino.h>

// ================= ピン定義 =================
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

// ================= チャンネルマップ =================
const uint8_t ch_IR[16] = {
  8,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9
};

const uint8_t ch_LINE[32] = {
  11,12,13,14,15,10,9,8,
  31,30,29,28,24,25,26,27,
  16,17,18,19,20,21,22,23,
  3,2,1,0,4,5,6,7
};

// ================= バッファ =================
uint16_t rawData_LINE[32];
int16_t  sensorValue_LINE[32];

// ================= MUX制御 =================
void initMUX(){
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
}

void selectChannel_16(uint8_t ch) {
  ch &= 0x0F;
  digitalWrite(S0,  ch        & 0x01);
  digitalWrite(S1, (ch >> 1)  & 0x01);
  digitalWrite(S2, (ch >> 2)  & 0x01);
  digitalWrite(S3, (ch >> 3)  & 0x01);
}

// ================= ステートマシン =================
enum LineState {
  SET_CH,
  WAIT_SETTLE,
  DUMMY_READ,
  READ,
  NEXT_CH
};

LineState lineState = SET_CH;

uint8_t currentCh = 0;
bool readCOM2 = false;
uint32_t t_us = 0;

// ================= 非ブロッキング読み取り =================
void readLINE_async(){

  switch(lineState){

    case SET_CH:
      selectChannel_16(currentCh);
      t_us = micros();
      lineState = WAIT_SETTLE;
      break;

    case WAIT_SETTLE:
      if(micros() - t_us >= 8){   // ← 調整ポイント（5〜15µs）
        lineState = DUMMY_READ;
      }
      break;

    case DUMMY_READ:
      if(!readCOM2){
        analogRead(LINE_COM1);
      }else{
        analogRead(LINE_COM2);
      }
      lineState = READ;
      break;

    case READ:
      if(!readCOM2){
        rawData_LINE[currentCh] = analogRead(LINE_COM1);
      }else{
        rawData_LINE[16 + currentCh] = analogRead(LINE_COM2);
      }
      lineState = NEXT_CH;
      break;

    case NEXT_CH:
      currentCh++;

      if(currentCh >= 16){
        currentCh = 0;

        if(!readCOM2){
          readCOM2 = true;   // COM2へ
        }else{
          readCOM2 = false;  // 1周完了

          // 並べ替え＋移動平均
          for(uint8_t i = 0; i < 32; i++){
            int16_t newVal = rawData_LINE[ch_LINE[i]];
            sensorValue_LINE[i] =
              (sensorValue_LINE[i] + newVal) >> 1;
          }

          // デバッグ出力
          Serial.print("LINE:");
          for(uint8_t i = 0; i < 32; i++){
            Serial.print(",");
            Serial.print(sensorValue_LINE[i] / 10);
          }
          Serial.println();
        }
      }

      lineState = SET_CH;
      break;
  }
}

// ================= セットアップ =================
void setup(){
  Serial.begin(115200);
  initMUX();

  // ADC安定化（ESP32対策）
  for(int i=0;i<10;i++){
    analogRead(LINE_COM1);
    analogRead(LINE_COM2);
  }
  pinMode(27, OUTPUT);
}

// ================= ループ =================
void loop(){
  digitalWrite(27, HIGH);
  readLINE_async();

  // 他の処理をここに書ける（モータ・通信など）
}