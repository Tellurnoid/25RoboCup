#include "UART.h"
// #define US_PIN 5   // Trig と Echo を 1kΩでつないだピン
const int NUM_SENSOR = 4;
const int pin[NUM_SENSOR] = {5, 4, 3, 2};

int16_t dis[NUM_SENSOR];      // ESP32へ送る距離（cm）

void setup() {
  initUART();
}

int16_t readUltrasonic1Pin(uint8_t pin) {

  // --- 出力モードでトリガ発射 ---
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);

  // --- 入力モードに切り替えて Echo 待ち ---
  pinMode(pin, INPUT);

  // ✅ タイムアウト付き pulseIn（最大10ms ≒ 170cm）
  long t = pulseIn(pin, HIGH, 10000);

  // ✅ 反射なしは -1 を返す
  if (t == 0) return -1;

  // ✅ 距離へ変換（mm）
  return (int16_t)(t / 5.8);
}


void loop() {
  for (int i = 0; i < NUM_SENSOR; i++) {
    dis[i] = readUltrasonic1Pin(pin[i]);
  }

  // for (int i = 0; i < NUM_SENSOR; i++) {
  //   Serial.print(dis[i]);
  //   Serial.print(" : ");
  // }
  // Serial.println(" ");
  // ✅ デバッグ表示（確認用）
  // Serial.println(dis);
  UART();
  // ✅ ここで ESP32 へ UART 送信するなら sendPacket() を呼ぶ
  // sendPacket(Serial, sp2); など

  delay(10);  // 100Hz（これ以上速くするとセンサが不安定になる）
}
