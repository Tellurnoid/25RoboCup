#pragma once

#include "Instance.h"

class State {
  public:
    struct statePacket {
      bool gyro_calibration;
      uint8_t line_calibration;
    };

    statePacket sp;
};