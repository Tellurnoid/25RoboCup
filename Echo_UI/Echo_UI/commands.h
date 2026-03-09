#pragma once
#include <Arduino.h>
#include "UART.h"
class Command {
  private:
    uint8_t ack = 0;
    uint32_t last_time = 0;
    uint32_t last_send_time = 0;
    // static constexpr uint32_t SEND_CYCLE = 100; // ms
    static constexpr uint32_t FAIL_TIME = 1000;

  public:
    void sendCommand(uint8_t id, uint8_t this_ack, uint8_t content_id, int16_t content);
    void command(uint8_t id, uint8_t content_id, int16_t content);
};