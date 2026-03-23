#pragma once

#include <Arduino.h>
#include "IMU.h"
#include "State.h"
#include "Sound.h"
#include "functions.h"

#include "UART.h" // UARTは最後

#include "Attack.h"
#include "Defense.h"

class Gyro; // 前方宣言
class UART;
class Data;
class Sound;
class State;
extern Gyro gyro;
extern UART uart;
extern Data data;
extern Sound sound;
extern State state;
//アタッカーのみ
// class Ball;
// class Line;
 class Attack;
// extern Ball ball;
// extern Line line;
 extern Attack attack;

//ディフェンスのみ

// class Echo;
class Defense;
extern Defense defense;