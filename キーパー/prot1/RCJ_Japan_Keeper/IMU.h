#pragma once
#include <Arduino.h>
#include "Instance.h"
#include <Wire.h>
#include <math.h>

class Gyro {
  private:
    float Kp = 10.0;
    float Kd = 0.2;


    #define MPU6050_ADDR 0x68
    float zero_angle = 0;
    volatile float angleZ = 0.0;
    volatile float dpsZ = 0.0;

    float gyro_offset_z = 0;

    void writeMPU(uint8_t reg, uint8_t data);
    void readMPU(int16_t &ax,int16_t &ay,int16_t &az,
             int16_t &gx,int16_t &gy,int16_t &gz);

  public: 
    void mpuTask();
    void initGyro();
    void updateGyro();
    float getAngle();
    float getDps();
    void resetGyro();
    int controlAngle(int target_angle);
};
