#pragma once

#include <Arduino.h>
#include <math.h>
#include "IMU.h"
#include "State.h"
#include "Sound.h"
#include "Motor.h"
#include "functions.h"

#include "UART.h" // UARTは最後

#include "Attack.h"
#include "Defense.h"

class Gyro; // 前方宣言
class UART;
class Data;
class State;
class Sound;
class Kicker;
class Sensor;

// class Ball;
// class Line;
// class Field;
class Attack;
class Defense;

extern Gyro gyro;
extern UART uart;
extern Data data;
extern State state;
extern Sound sound;
extern Kicker kicker;
extern Sensor sensor;

// extern Ball ball;
// extern Line line;
// extern Field field;
extern Attack attack;
extern Defense defense;