#include "Instance.h"
#include "IMU.h"

#include <Wire.h>
#include "pico/multicore.h"

void core1GyroTask() {
  gyro.mpuTask();
}

void Gyro::writeMPU(uint8_t reg, uint8_t data){
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void Gyro::readMPU(int16_t &ax,int16_t &ay,int16_t &az,
             int16_t &gx,int16_t &gy,int16_t &gz){

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU6050_ADDR,14);

  ax = Wire.read()<<8 | Wire.read();
  ay = Wire.read()<<8 | Wire.read();
  az = Wire.read()<<8 | Wire.read();

  Wire.read();
  Wire.read();

  gx = Wire.read()<<8 | Wire.read();
  gy = Wire.read()<<8 | Wire.read();
  gz = Wire.read()<<8 | Wire.read();
}

void Gyro::mpuTask(){

  int16_t ax,ay,az,gx,gy,gz;

  Serial.println("MPU calibration");
  calibration = true;

  for(int i=0;i<2000;i++){

    readMPU(ax,ay,az,gx,gy,gz);

    gyro_offset_z += (float)gz/16.4;

    delay(1);
  }

  gyro_offset_z/=2000;

  Serial.println("done");
  calibration = false;

  uint32_t lastMicros = micros();

  while(true){

    readMPU(ax,ay,az,gx,gy,gz);

    float dps = (float)gz/16.4;

    uint32_t now = micros();
    float dt = (now-lastMicros)/1000000.0;
    lastMicros = now;

    float newAngle = angleZ + (dps-gyro_offset_z)*dt;

    noInterrupts();
    angleZ = newAngle;
    dpsZ = dps;
    interrupts();

    delay(5);
  }
}

float Gyro::getAngle(){

  float a;

  noInterrupts();
  a = -(zero_angle - (-angleZ));
  interrupts();

  while(a>180) a-=360;
  while(a<-180) a+=360;

  return a;
}

float Gyro::getDps(){

  float v;

  noInterrupts();
  v = dpsZ;
  interrupts();

  return v;
}


void Gyro::resetGyro() {
  zero_angle = -angleZ;
}

void Gyro::init() {
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();

  delay(100);

  writeMPU(0x6B,0x00);
  writeMPU(0x1B,0x18);
  writeMPU(0x1C,0x00);

  delay(100);

  multicore_launch_core1(core1GyroTask);
}

void Gyro::update() {
  state.sp.gyro_calibration = calibration;
}

int Gyro::controlAngle(int target_angle) {
  float error = -((float)target_angle - getAngle());

  int output = Kp * error - Kd * getDps();
  return output;
}