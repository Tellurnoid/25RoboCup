#pragma once

#include "IMU.h"
#include "Main.h"
#include "UART.h"

extern Gyro gyro;
extern UART uart;
extern Data data;
extern Ball ball;
extern Line line; // extern では実体を描くだけだから、そのまま
extern Attack attack;