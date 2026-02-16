#ifndef IR_H
#define IR_H

struct Vector {
  float x;
  float y;
};

struct BallResult {
  float angle;
  float distance;
};

void initIR();
void IR();

extern int ball_angle;
extern int ball_distance;
extern int ball_max;

#endif
