#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <Arduino.h>

struct Vector {
  float x;
  float y;
};

Vector makeV(float deg, float speed);
Vector addV(Vector a, Vector b);
Vector assembleV(Vector a, Vector b, float max_len);
Vector normalizeV(Vector v);
Vector changeLenV(Vector v, int len_v);
float lenV(Vector v, int deg);
int angleDiff(int a, int b);
float wrap180(float angle);
float wrap360(float angle);
float convertAngle(float theta);
float reverseAngle(float theta_p);
#endif