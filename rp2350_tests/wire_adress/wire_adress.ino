#include <Wire.h>
//メモ
//pico MPU
//GP4 SDA
//GP5 SCL
//が最も安心！

//以下のコードを追加して
// Wire.setSDA(16);
// Wire.setSCL(17);
//pico MPU
//GP16 SDA
//GP17 SCL
//でも動作
void setup() {
  Serial.begin(115200);
  while (!Serial);   // Picoではシリアル待機（任意）
// Wire.setSDA(28);
// Wire.setSCL(27);
  Wire.begin();      // デフォルトI2C開始
  Serial.println("I2C Scanner Start");
}

void loop() {
  byte error, address;
  int deviceCount = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Found device at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      deviceCount++;
    }
  }

  if (deviceCount == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.println("Scan complete");
  }

  delay(3000);
}
