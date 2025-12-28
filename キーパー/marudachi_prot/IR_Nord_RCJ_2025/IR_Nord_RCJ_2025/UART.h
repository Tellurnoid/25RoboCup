#ifndef UART_H
#define UART_H

struct ReadPacketData0 { int angleZ; };
struct ReadPacketData1 { int16_t cx; int16_t cy; int16_t area; };
struct ReadPacketData2 { int16_t dis1; int16_t dis2; int16_t dis3; int16_t dis4; };

struct SendPacketData0 { int ball_angle; int ball_distance; int ball_max; int16_t dis1; int16_t dis2; int16_t dis3; int16_t dis4; int16_t cx;};
struct SendPacketData1 { int ball_angle; int goal_angle; };
struct SendPacketData2 { int16_t serial_date; };

void initUART();
void UART();

// extern int dis;

#endif
