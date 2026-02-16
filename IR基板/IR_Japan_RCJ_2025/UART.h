#ifndef UART_H
#define UART_H

struct ReadPacketData0 { };//メインとの通信
struct ReadPacketData1 { int16_t c_x; int16_t c_y; int16_t c_s; };//カメラとの通信
struct ReadPacketData2 { int16_t dis1; int16_t dis2; int16_t dis3; int16_t dis4; };//UI,超音波

struct SendPacketData0 {
                        int16_t ball_angle;
                        int16_t ball_distance; 
                        int16_t ball_max; 
                        int16_t dis1; 
                        int16_t dis2; 
                        int16_t dis3; 
                        int16_t dis4; 
                        int16_t c_x; 
                        int16_t c_y;
                        int16_t c_s;
                       };

struct SendPacketData1 {};// int ball_angle; int goal_angle;
struct SendPacketData2 {};// int16_t serial_date;
void initUART();
void UART();

// extern int dis;

#endif
