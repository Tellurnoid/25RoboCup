#pragma once

#include <Arduino.h>
#include "Instance.h"
#include "functions.h"

class Line {
  private:
    int16_t line_angle;
    int16_t line_distance;

    int16_t last_field_line_angle = 0;
    int16_t last_line_angle = 0;
    static constexpr float AVLID_LINE_Kp = 1;

    static constexpr int8_t KEEP_LINE_DIS = 50;
    static constexpr float KEEP_LINE_Kp = 3.0;
    static constexpr float KEEP_LINE_Kd = 0;

    static constexpr int16_t AVLID_LINE_MIN_SPEED = 100;
    static constexpr int16_t AVOID_LINE_SPEED = 500;
    static constexpr int16_t BORDER_OVER_LINE = 130;

    bool over_line = false;

  public:
    int16_t serial_angle;

    int getFieldLineAngle();
    int getFieldLineDistance();

    Vector cancelV(int16_t field_line_angle, Vector cancel_v);

    Vector lineV(int16_t line_angle, Vector cancel_v);

    void updataLine();
};

class Ball {
  private:
    static constexpr int16_t BALL_SPEED = 500;
    static constexpr uint8_t KdigDis[17] = {0, 0, 0, 20, 20, 20, 30, 60, 60, 90, 90, 90, 90, 90, 90, 90, 90};
  public:
    Vector ballV(int16_t ball_angle, int16_t ball_distance);
};

class Field {
  private:
    static constexpr int16_t X_MIN_FIELD = 1200;
    static constexpr int16_t Y_MIN_FIELD = 1800;

    int16_t last_position_x;
    int16_t last_position_y;
  public:
    int16_t position_x;
    int16_t position_y;

    void getFeildPosition(int echo_right, int echo_left, int echo_plus, int echo_minus, int move_x, int move_y, int angle);
};

class Attack {
  private:
    uint8_t ack = 0;
  public:
    enum Behavior {
      BALL_CHASE = 0,
      SHOOT,
      STOP,
      OFF
    };

  private:
    // Gyro my_gyro;
    // Ball my_ball;
    // Line my_line;

    Behavior behavior = OFF;

    Vector attack_v;
    int attack_rotate;

    struct AttackData {
      Vector v;
      int rotate;
    };

  public:
    void initAttack();

    void setBehavior(Behavior b);

    void updateAttack();

    AttackData getAttackData() {
      return {attack_v, attack_rotate};
    }

    // AttackData main_data;

  private:
    static constexpr uint16_t MAX_SPEED = 700;

    void ballChase();

    void shoot();

    void stop();

    void off();
};
