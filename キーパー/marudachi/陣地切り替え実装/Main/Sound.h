#pragma once

#include "Instance.h"
class Sound{
    private:
      static constexpr uint8_t beep = 13;
    public:
      void init();
      void success();
      void single();
      void update();
};