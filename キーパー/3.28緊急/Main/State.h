#pragma once

#include "Instance.h"

class State {
  public:
    struct StatePacket {
      bool gyro_calibration;
      bool line_calibration;
      bool near_wall;
      bool on_line;
    };

    StatePacket sp;
};