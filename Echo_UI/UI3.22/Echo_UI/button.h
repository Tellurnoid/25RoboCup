#pragma once
#include <Arduino.h>

class Button {
  private:
    static constexpr uint8_t NUM_PINS = 4;  // 関数内ではなく外で書かれたstaticはこのクラスからできたオブジェクト間で共有される
    static constexpr uint8_t pins[NUM_PINS] = {2, 3, 6, 7};

  public:
    static constexpr int8_t ENTER  = 2;
    static constexpr int8_t BACK   = 7;
    static constexpr int8_t RIGHT  = 3;
    static constexpr int8_t LEFT   = 6;
    int8_t states[NUM_PINS] = {0};
    void init();
    int8_t read(int8_t pin);
};

