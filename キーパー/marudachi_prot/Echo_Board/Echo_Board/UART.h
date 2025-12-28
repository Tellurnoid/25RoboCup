#ifndef UART_H
#define UART_H

struct ReadPacketData0 { int ball_angle; int goal_angle; };

struct SendPacketData0 { int16_t dis1; int16_t dis2; int16_t dis3; int16_t dis4; };

void initUART();
void UART();

#endif
