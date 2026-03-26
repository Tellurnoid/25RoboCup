#include <Arduino.h>
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
int16_t rawData_IR[16];
int16_t rawData_LINE[32];
// 読み取り結果格納用(半時計回り)
int16_t sensorValue_IR[16];
int16_t sensorValue_LINE[32];
long calcValue_LINE[32];

void setup(){
  Serial.begin(115200);
  initMUX();
  while(!Serial);
  calibration_line();
}

void loop(){
  //  readMUX();
  //  printAllMUX();
  analogWrite(27, 255);

  //readMUX_onlyIR_debug2();
  readMUX_onlyLINE_debug_relative2();
   //readMUX_onlyIR_debug();
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
        if(sensorValue_IR[ch]/10 < 10){
          Serial.print("  ");
        }
        else if(sensorValue_IR[ch]/10 < 100){
          Serial.print(" ");
        }
        Serial.print(sensorValue_IR[ch]/10);
        
      }
      Serial.println();
}
void readMUX_onlyIR_debug2(){
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
        Serial.print(" ");
        if(sensorValue_IR[ch]/10 == 409){
          Serial.print(" ");
        }
        else {
          Serial.print("*");
        }
      }
      Serial.println();
}
// void readMUX_onlyLINE_debug(){
//   //読む
//   for(uint8_t ch=0; ch<32; ch++){
//     selectChannel_16(ch);
//     delayMicroseconds(50);
//     //まず一つの配列、rawDataに流し込む
//     analogRead(LINE_COM1);
//     analogRead(LINE_COM1);
//     analogRead(LINE_COM1);
//     sensorValue_LINE[ch]    = analogRead(LINE_COM1);

//            Serial.print(",  LINE: ");
//       for (uint8_t ch = 0; ch < 32; ch++) {
//         Serial.print(",");
//         Serial.print(sensorValue_LINE[ch]);
//       }
//       Serial.println();
//   }
// }
void readMUX_onlyLINE_debug(){

  for(uint8_t ch=0; ch<32; ch++){
    selectChannel_16(ch);
    delayMicroseconds(50);
    analogRead(LINE_COM1);
    sensorValue_LINE[ch] = analogRead(LINE_COM1);
  }

  Serial.print("LINE:");

  for (uint8_t i = 0; i < 32; i++) {
      Serial.print(",");
        if(sensorValue_LINE[i] < 10){Serial.print("   ");}
        else if(sensorValue_LINE[i] < 100){Serial.print("  ");}
        else if(sensorValue_LINE[i] < 1000){Serial.print(" ");}
      Serial.print(sensorValue_LINE[i]/100);
  }

  Serial.println();
}

void calibration_line(){
  int how_much_count = 500;
 for(int count=0; count<how_much_count; count++){
    //読む
  for(uint8_t ch=0; ch<16; ch++){
    analogWrite(27, 200);
    selectChannel_16(ch);
     uint32_t start = micros();
     delay(1);
    analogRead(LINE_COM1);//捨て読み
    rawData_LINE[ch] = analogRead(LINE_COM1);
    //delayMicroseconds(10);
    analogRead(LINE_COM2);//捨て読み
    rawData_LINE[16+ch] = analogRead(LINE_COM2);
  }
  for(uint8_t i = 0; i < 32; i++){
     calcValue_LINE[i] += rawData_LINE[ch_LINE[i]];
  }
  Serial.println(count);
 }
  Serial.println("--------------------calc--------------------");
  for(uint8_t i = 0; i < 32; i++){
     calcValue_LINE[i] = calcValue_LINE[i]/how_much_count;
     Serial.print(calcValue_LINE[i]);
     Serial.print(",");
  }
  Serial.println();
  Serial.println("--------------------------------------------");
  Serial.println();
}

void readMUX_onlyLINE_debug_relative(){
    //読む
  for(uint8_t ch=0; ch<16; ch++){
    selectChannel_16(ch);
    //delay(1);
     //delayMicroseconds(2000);
     uint32_t start = micros();
     while (micros() - start < 30) {}
    analogRead(LINE_COM1);//捨て読み
    rawData_LINE[ch] = analogRead(LINE_COM1);
    analogRead(LINE_COM2);//捨て読み
    rawData_LINE[16+ch] = analogRead(LINE_COM2);
    // delayMicroseconds(300);
   // delay(20);
//  start = micros();
// while (micros() - start < 500) {}  // 0.5ms

  }
  for(uint8_t i = 0; i < 32; i++){
     sensorValue_LINE[i] = calcValue_LINE[i] - rawData_LINE[ch_LINE[i]];
     if(sensorValue_LINE[i] > 0){Serial.print("+");}
     if(abs(sensorValue_LINE[i]) < 10){Serial.print("  ");}
     Serial.print(sensorValue_LINE[i]/100);Serial.print(",");
  }  
  Serial.println();
}


void readMUX_onlyLINE_debug_relative2(){
    //読む
  for(uint8_t ch=0; ch<16; ch++){
    selectChannel_16(ch);
    //delay(1);
     //delayMicroseconds(2000);
     uint32_t start = micros();
     while (micros() - start < 30) {}
     analogRead(LINE_COM1);//捨て読み
    rawData_LINE[ch] = analogRead(LINE_COM1);
    analogRead(LINE_COM2);//捨て読み
    rawData_LINE[16+ch] = analogRead(LINE_COM2);
    // delayMicroseconds(300);
   // delay(20);
  }
  for(uint8_t i = 0; i < 32; i++){
     sensorValue_LINE[i] = calcValue_LINE[i] - rawData_LINE[ch_LINE[i]];
     if(abs(sensorValue_LINE[i]) > 200){
      Serial.print("*");
      //Serial.print(i);
      //Serial.print(":");
      //Serial.print(sensorValue_LINE[i]);
     }else{
      Serial.print(" ");
     }
     Serial.print("");
  }  
  Serial.println();
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


  // void readMUX_onlyLINE(){
  // //読む
  // for(uint8_t ch=0; ch<16; ch++){
  //  selectChannel_16(ch);
  //   analogRead(LINE_COM1);//捨て読み
  //   rawData_LINE[ch] = analogRead(LINE_COM1);
  //   delayMicroseconds(5);
  //   analogRead(LINE_COM2);//捨て読み
  //   rawData_LINE[16+ch] = analogRead(LINE_COM2);
  //   delayMicroseconds(5);
  // }  
  //    for(uint8_t i = 0; i < 32; i++){
  //    sensorValue_LINE[i] = rawData_LINE[ch_LINE[i]];
  //  }
  // }

  // void lineSensor_calibration(){
  //   uint16_t cal_times = 1000;
  //   long sensorValue_LINE_forCal[32];
  //   for(uint16_t i=0; i<cal_times; i++){
  //     for(int ch=0; ch<16; ch++){
  //       selectChannel_16(ch);
  //       sensorValue_LINE_forCal[i] = 
  //     }
  //     for(int ch=16; ch<32; ch++){
  //       selectChannel_16(ch-16);
  //       sensorValue_LINE_forCal[i] = 
  //     }
  //   }
  // }