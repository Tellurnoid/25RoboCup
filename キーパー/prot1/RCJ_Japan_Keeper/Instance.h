#pragma once
#include <Arduino.h>
#include "functions.h"
#include "UART.h"
#include "keeper_main.h"
#include "IMU.h"
#include "Attack.h"


#include "sound.h"
//#include "Main.h"

extern Data data;
extern Gyro gyro;
extern Echo echo;
extern UART uart;
extern AngleOperation angle;
extern Sound sound;
extern Keeper keeper;

extern Ball ball;
extern Line line; // extern では実体を描くだけだから、そのまま
extern Attack attack;
// extern では実体を描くだけだから、そのまま


/*

        AngleOperation angle;
        Sound sound;
        Gyro gyro;
*/