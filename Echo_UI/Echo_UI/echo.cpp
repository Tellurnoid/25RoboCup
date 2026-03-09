#include "echo.h"
#include <Arduino.h>

    int16_t Echo::getEchoValue(int pin) {
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

    void Echo::read() {
      for (int i = 0; i < NUM_SENSORS; i++) {
        echoValues[i] = getEchoValue(pins[i]);
      }
    }
