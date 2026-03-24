#pragma once

#include "Instance.h"

class Kicker {
  private:
    #define IN1 10
    #define IN2 12

    #define SENSOR 27

    static constexpr uint16_t CHARGE_BORDER = 500;

    bool kicker_charge = false;
    bool kicking = false;

  public:
    void init();
    void update();
    void charge();
    void kick();
};

class Sensor {
  private:
    #define BALL_ZONE 26
    static constexpr uint16_t BALL_CATCH_BORDER = 20;

    bool ball_catch = false;

  public:
    void init();
    void update();
};