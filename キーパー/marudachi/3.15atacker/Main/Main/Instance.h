#pragma once

#include <Arduino.h>
#include "IMU.h"
#include "Attack.h"
#include "UART.h"
#include "functions.h"

extern Gyro gyro;
extern UART uart;
extern Data data;
extern Ball ball;
extern Line line; // extern では実体を描くだけだから、そのまま
extern Attack attack;