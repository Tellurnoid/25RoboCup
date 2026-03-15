#pragma once
#include <Arduino.h>
#include "Instance.h"
class Sound{
    public:
      void init();
      void success();
      void single();
    private:
      static constexpr uint8_t beep = 23;
};
