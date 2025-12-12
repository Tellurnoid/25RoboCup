#ifndef MOVE_H
#define MOVE_H

void initMotor();
void motor(char motor, int pwm);

#include <math.h>
// #include "functions.h"

void moveVector(Vector v, int omega);

#endif
