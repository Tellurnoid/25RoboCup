#pragma once
#include <Arduino.h>

class Sound{
    private:
    public:
      static constexpr uint8_t pin = 27;
      void init();    
      void single();
      void success();
      void enter();
      void error();
      void cursor();
      void back();
      void start();
      void user(int your_tone,int length_on, int length_off,int times);
};