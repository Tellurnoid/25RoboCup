#ifndef APP_H
#define APP_H
#include <Arduino.h>
#include <U8x8lib.h>

// Declare the global display object (defined once in node.ino)
extern U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8;

class app{
    public:
      void homeMenu();
      void LEDBrightness();
      void SerialMonitor();
    private:
};

#endif