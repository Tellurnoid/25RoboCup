#ifndef IMU_H
#define IMU_H

#include <Wire.h>
#include <math.h>
// Core0で更新されるZ軸角度を他のファイルから参照可能に
extern volatile float angleZ; // angleZは右が-、左が+
// extern float dpsX, dpsY, dpsZ;


void initIMU();
void IMU();
float rotatePID(int mode, int target_angle);

#endif
