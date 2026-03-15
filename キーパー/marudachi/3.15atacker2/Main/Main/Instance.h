#pragma once

#include <Arduino.h>
#include "IMU.h"
#include "State.h"
#include "Sound.h"
#include "functions.h"

#include "UART.h" // UARTは最後

#include "Attack.h"

class Gyro; // 前方宣言
class UART;
class Data;
class State;
class Sound;

class Ball;
class Line;
class Attack;

extern Gyro gyro;
extern UART uart;
extern Data data;
extern State state;
extern Sound sound;

extern Ball ball;
extern Line line;
extern Attack attack;