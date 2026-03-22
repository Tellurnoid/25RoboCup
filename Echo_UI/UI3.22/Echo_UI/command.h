#pragma once

#include <Arduino.h>
// #include "Instance.h"
#include "UART.h"

class Command {
  private:
    uint8_t ack = 0;

    void makeCommand(uint8_t id, uint8_t this_ack, uint8_t content_id, int16_t content);

  public:
    void sendCommand(uint8_t id, uint8_t content_id, int16_t content);
};