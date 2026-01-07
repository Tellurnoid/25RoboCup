//ボードマネージャーのURL↓
//https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
//ボードマネージャー　Raspberry Pi Pico/RP2040　がインストール済みか確認してください
#include <SerialPIO.h>

SerialPIO pioUartA(1,2);
SerialPIO pioUartB(3,4);
SerialPIO pioUartC(10, 11);
SerialPIO pioUartD(12, 13);

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);

  pioUartA.begin(115200);
  pioUartB.begin(115200);
  pioUartC.begin(115200);
  pioUartD.begin(115200);
}
char dataSerial2 = 0;
char dataA = 0;
char dataB = 0;
char dataC = 0;
char dataD = 0;

void loop() {
  Serial2.println('1');
  pioUartA.println('1');
  pioUartB.println('1');
  pioUartC.println('1');
  pioUartD.println('1');
  delay(50);
    if(Serial2.available()){
    dataSerial2 = Serial2.read();
    Serial.print(",Serial2:");
    Serial.println(dataSerial2);
  }
  if(pioUartA.available()){
    dataA = pioUartA.read();
    Serial.print(",A:");
    Serial.println(dataA);
  }
    if(pioUartB.available()){
    dataB = pioUartB.read();
    Serial.print(",B:");
    Serial.println(dataB);
  }
    if(pioUartC.available()){
    dataC = pioUartC.read();
    Serial.print(",C:");
    Serial.println(dataC);
  }
    if(pioUartD.available()){
    dataD = pioUartD.read();
    Serial.print(",D:");
    Serial.println(dataD);
  }
  Serial2.println('1');
}


