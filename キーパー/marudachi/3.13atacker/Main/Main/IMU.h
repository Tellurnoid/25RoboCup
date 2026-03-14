#pragma once

#include <Wire.h>
#include <math.h>

// // Core0で更新されるZ軸角度を他のファイルから参照可能に
// extern volatile float angleZ; // angleZは右が-、左が+
// // extern float dpsX, dpsY, dpsZ;


// void initIMU();
// void IMU();
// float rotatePID(int mode, int target_angle);

class Gyro {
  private:
    float Kp = 10.0;
    float Kd = 0.1;

  public: 
    void initGyro();
    void updateGyro();
    float getAngle();
    float getDps();
    int controlAngle(int target_angle);
};
