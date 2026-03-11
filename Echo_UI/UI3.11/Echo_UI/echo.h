#pragma once
#include <Arduino.h>

class Echo {
  private:
    static constexpr uint8_t NUM_SENSORS = 8;
    static constexpr uint8_t pins[NUM_SENSORS] = {11, 12, 13, 14, 15, 8, 9, 10};  // 前　

  public:
    int16_t echoValues[NUM_SENSORS] = {0};
    int16_t getEchoValue(int pin);
    void read();
};