//ボードマネージャーのURL↓
//https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
//ボードマネージャー　Raspberry Pi Pico/RP2040　がインストール済みか確認してください
#include <SerialPIO.h>

SerialPIO pioUartA(6, 7);
SerialPIO pioUartB(8, 9);
SerialPIO pioUartC(10, 11);
SerialPIO pioUartD(12, 13);

void setup() {
  Serial.begin(115200);

  pioUartA.begin(115200);
  pioUartB.begin(115200);
  pioUartC.begin(115200);
  pioUartD.begin(115200);
}

char dataA = 0;
char dataB = 0;
char dataC = 0;
char dataD = 0;

void loop() {
  pioUartA.println('1');
  pioUartB.println('1');
  pioUartC.println('1');
  pioUartD.println('1');
  delay(50);

  if(pioUartA.available()){
    dataA = pioUartA.read();
    Serial.print(",A:");
    Serial.print(dataA);
  }
    if(pioUartB.available()){
    dataB = pioUartB.read();
    Serial.print(",B:");
    Serial.print(dataB);
  }
    if(pioUartC.available()){
    dataC = pioUartC.read();
    Serial.print(",C:");
    Serial.print(dataC);
  }
    if(pioUartD.available()){
    dataD = pioUartD.read();
    Serial.print(",D:");
    Serial.print(dataD);
  }
  Serial.println();
}
