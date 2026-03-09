#pragma once
#include <Arduino.h>

class Button {
  private:
    static constexpr uint8_t NUM_PINS = 4;  // 関数内ではなく外で書かれたstaticはこのクラスからできたオブジェクト間で共有される
    static constexpr uint8_t pins[NUM_PINS] = {2, 3, 6, 7};

  public:
    static constexpr uint8_t KEY_ENTER  = 0;
    static constexpr uint8_t KEY_BACK   = 1;
    static constexpr uint8_t KEY_RIGHT  = 2;
    static constexpr uint8_t KEY_LEFT   = 3;
    int8_t buttonStates[NUM_PINS] = {0};
    void init();
    void read();
};

