#pragma once

#include <Arduino.h>
#include "Instance.h"
#include "functions.h"

class Line {
  private:
    int16_t line_angle;
    int16_t line_distance;

    int16_t last_line_angle = 0;
    int16_t last_on_line_angle = 0;

    static constexpr int16_t AVOID_LINE_SPEED = 500;
    static constexpr int16_t BORDER_OVER_LINE = 130;

    int16_t field_line_angle;
    int16_t field_line_distance;

    bool on_line = false;
    bool on_t_line = false;
    bool over_line = false;
    bool in_line = false;

  public:
    int getFieldLineAngle();
    bool getOverLine();
    Vector lineV();
    Vector cancelV(Vector v);
    void update();
};

class Ball {
  private:
    int16_t ball_angle;
    int16_t ball_distance;
    static constexpr int MOVE_SPEED = 500;  // 500が自己位置推定なしでラインアウトしない最大
    // static constexpr uint8_t KdigDis[17] = {15, 15, 15, 25, 25, 25, 30, 60, 60, 90, 90, 90, 90, 90, 90, 90, 90};
    static constexpr uint8_t KdigDis[17] = {45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 60, 60, 60, 60, 60, 60, 60};
    // static constexpr uint8_t borderDis[17] = {30, 30, 30, 30, 30, 30, 30, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45};
    // static constexpr uint8_t borderDis[17] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60};
    static constexpr uint8_t borderDis[17] = {60, 60, 60, 60, 60, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45};

    static constexpr int FRONT_RANGE = 15;

    static constexpr int16_t BALL_D_INTERVAL = 50;  // ms
    float ball_d;
    uint32_t last_time;
    int last_ball_angle;
    float last_ball_d = 0.0f;

    float Kd = 0.7f;  // スピード500の時はKd=0.4

    bool ball_found = false;

  public:
    void update();
    float getBallD();
    Vector ballV(int SPEED);
};

class Field {
  private:
    static constexpr uint16_t FIELD_X = 1700;  // 実測 + 1000
    static constexpr uint16_t FIELD_Y = 2300;
    static constexpr int16_t FIELD_TOLERANCE = 200;

    static constexpr uint8_t NUM_ECHOS = 8;
    int16_t echoValues[NUM_ECHOS];
    int16_t echoAngles[NUM_ECHOS] = {0, 45, 90, 135, 180, 225, 270, 315};

    int16_t lastEchoValues[NUM_ECHOS] = {0};
    int16_t lastEchoDiff[NUM_ECHOS] = {0};


    float move_x;
    float move_y;
    float last_move_x = 0.0f;
    float last_move_y = 0.0f;

    float move_k = 1.0f;

    static constexpr int16_t ZERO_OVER_ERROR = 100; // 片側


    int16_t robot_angle;

    struct Position {
      int16_t x;
      int16_t y;
    };
    Position field_position;

    struct PositionLost {
      bool x;
      bool y;
    };
    PositionLost position_lost = {false, false};


    int16_t crimp[NUM_ECHOS] = {-2};
    static constexpr uint8_t CRIMP_BORDER = 200;

    bool crimpFlags[NUM_ECHOS] = {false};

    enum Location {
      BOTH_ECHO = 0,
      SINGLE_ECHO,
      MOVEMENT
    };
    Location location_x;
    Location location_y;

    static constexpr float brake_Kd = 1.5f;
    float world_velocity_x;
    float world_velocity_y;
    
  public:
    void update();

    int getEchoForwardIndex();

    void calculateCrimp();
    void resetCrimpFlags();

    void calculatePosition();

    // Position getFieldPosition();
    PositionLost getPositionLost();

    Vector brakeV();
};

class Attack {
  private:
    // uint8_t ack = 0;

    Ball ball;
    Line line;
    Field field;

  public:
    enum Behavior {
      BALL_CHASE = 0,
      BALL_CHASE_WITH_KICKER,
      SHOOT,
      KEEPER_DASH,
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

    void update();

    void startKeeperDash();

  private:
    static constexpr uint16_t MAX_SPEED = 800;

    bool keeper_dashing = false;
    bool first_line = false;

    uint16_t keeper_dash_time_limit =  1000;
    unsigned long start_keeper_dash_time = 0;

    void ballChase();

    void shoot();

    void keeperDash();

    void stop();

    void off();
};