#include <ESP32Servo.h>

Servo esc;

const int ESC_PIN = 27;

void setup() {
  esc.attach(ESC_PIN);

  // ESC初期化（アーミング）
  esc.writeMicroseconds(1000);  // 最小スロットル
  delay(3000);                  // ESCの起動音待ち
}

void loop() {
  // 回転開始（速度指定）
  esc.writeMicroseconds(1800);  // 1100〜2000で速度調整
  delay(5000);

  // 停止
  esc.writeMicroseconds(1000);
  delay(5000);
}
