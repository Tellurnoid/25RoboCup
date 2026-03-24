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

    void update();
};

class Ball {
  private:
    int16_t ball_angle;
    int16_t ball_distance;
    static constexpr int16_t BALL_SPEED = 700;
    static constexpr uint8_t KdigDis[17] = {15, 15, 15, 25, 25, 25, 30, 60, 60, 90, 90, 90, 90, 90, 90, 90, 90};
    static constexpr uint8_t borderDis[17] = {30, 30, 30, 30, 30, 30, 45, 45, 45, 45, 45, 45, 60, 60, 60, 60, 60};

    static constexpr int16_t BALL_D_INTERVAL = 50;  // ms
    float ball_d;
    uint32_t last_time;
    int last_ball_angle;

    float Kp = 20.0f;
    float Kd = 2.0f;
  public:
    void update();
    float getBallD();
    Vector ballV();
};

class Field {
  private:
    static constexpr uint16_t FIELD_X = 1800;
    static constexpr uint16_t FIELD_Y = 2400;
    static constexpr int16_t FIELD_TOLERANCE = 400;

    static constexpr uint8_t NUM_ECHOS = 8;
    int16_t echoValues[NUM_ECHOS];
    int16_t echoAngles[NUM_ECHOS] = {0, 45, 90, 135, 180, 225, 270, 315};

    int16_t lastEchoValues[NUM_ECHOS] = {0};
    int16_t lastEchoDiff[NUM_ECHOS] = {0};

    int16_t move_x;
    int16_t move_y;

    int16_t robot_angle;

    struct Position {
      int16_t x;
      int16_t y;
    };
    Position position;

    bool credibility = true;  // 現在座標が信頼できるものか

    int16_t crimp[NUM_ECHOS] = {0};
    static constexpr uint8_t CRIMP_BORDER = 100;
    
  public:
    void update();

    int getEchoForwardIndex();

    void getCrimp();

    // void getEchoPosition() {
    //   int forward_index = getEchoForwardIndex();
    //   Position echo_position;
    //   echo_position.x = 
    // }
};

class Attack {
  private:
    // uint8_t ack = 0;
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

    // struct AttackData {
    //   Vector v;
    //   int rotate;
    // };

  public:
    void init();

    void setBehavior(Behavior b);
    void startKeeperDash();
    void update();

    // AttackData getAttackData() {
    //   return {attack_v, attack_rotate};
    // }

    // AttackData main_data;

  private:
    static constexpr uint16_t MAX_SPEED = 700;

    void ballChase();

    void shoot();

    void stop();

    void off();
};