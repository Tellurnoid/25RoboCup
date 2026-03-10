#pragma once
#include <Arduino.h>

class Sound{
    private:
    public:
      static constexpr uint8_t pin = 27;
      void init();    
      void tone();
      void success();
      void error();
      void cursor();
      void back();
};