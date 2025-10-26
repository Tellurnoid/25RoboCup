/*
#include <HardwareSerial.h>

HardwareSerial mySerial(2); // UART2を使用

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17
  Serial.println("ESP32 ready!");
}

void loop() {
  // UnitVからの受信
  if (mySerial.available()) {
    String data = mySerial.readStringUntil('\n');
    Serial.print("Received from UnitV: ");
    Serial.println(data);
  }

  // 定期的に送信
  mySerial.println("Hello from ESP32!");
  delay(1000);
}
*/

#include <Arduino.h>

HardwareSerial &mySerial = Serial2; // UART2を使用する場合

void setup() {
  Serial.begin(115200);      // デバッグ用
  mySerial.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17 など適宜設定
}

void loop() {
  // 6バイト以上受信されるまで待つ
  if (mySerial.available() >= 6) {
    uint8_t buf[6];
    mySerial.readBytes(buf, 6);

    // ビッグエンディアンで復元
    uint16_t cx     = (buf[0] << 8) | buf[1];
    uint16_t cy     = (buf[2] << 8) | buf[3];
    uint16_t pixels = (buf[4] << 8) | buf[5];

    // デバッグ出力
    Serial.print("cx: "); Serial.print(cx);
    Serial.print(", cy: "); Serial.print(cy);
    Serial.print(", pixels: "); Serial.println(pixels);
  }
}
