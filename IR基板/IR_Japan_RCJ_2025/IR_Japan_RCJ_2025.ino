#include <Arduino.h>
#include "IR.h"
#include "UART.h" // UART()でいろんなファイルの変数使うから最後にincludeする必要あり

void setup() {
  initIR();
  initUART();
}
const int sensorPinsA[12] = {34, 35, 32, 33, 25, 26, 27, 14, 13, 4, 2, 15};//const int sensorPins[NUM_SENSORS] = {35, 32, 33, 25, 26, 27, 14, 2, 15};

void loop() {
  IR();
  UART();
  // Serial.print("angle:");Serial.print(ball_angle);
  // Serial.print(", distance:");Serial.print(ball_distance);
  // for(int i=0; i<12; i++){
  //   Serial.print(analogRead(sensorPinsA[i]));Serial.print(",");
  // }
  // Serial.println();

}
