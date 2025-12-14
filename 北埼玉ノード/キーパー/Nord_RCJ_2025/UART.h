#ifndef UART_H
#define UART_H

struct ReadPacketData0 { int ball_angle; int goal_angle; };
struct ReadPacketData1 { int ball_angle; int ball_distance; int ball_max; int16_t dis1; int16_t dis2; int16_t dis3; int16_t dis4; };
struct ReadPacketData2 { int16_t line_angle; int16_t line_dis; };

struct SendPacketData0 { int ball_angle; int ball_distance; };
struct SendPacketData1 { int angleZ; };
struct SendPacketData2 { int x; };

void initUART();
void UART();

extern int ball_angle;
extern int ball_distance;
extern int ball_max;
extern int16_t wall_dis;
extern int16_t line_angle;
extern int16_t line_dis;
extern int16_t dis[4];

#endif