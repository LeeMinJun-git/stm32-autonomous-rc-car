#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"

// 기본 주행 제어 함수 (수동 조작용)
void Motor_Forward(uint16_t speed);
void Motor_Backward(uint16_t speed);
void Motor_Left(uint16_t left_speed, uint16_t right_speed);   // 부드러운 좌회전
void Motor_Right(uint16_t left_speed, uint16_t right_speed);  // 부드러운 우회전
void Motor_Stop(void);                                        // 완전 정지

// 제자리 회전 제어 함수 (자율주행 좁은 공간 탈출용)
void Motor_Turn_Left_In_Place(uint16_t speed);  // 제자리 좌회전
void Motor_Turn_Right_In_Place(uint16_t speed); // 제자리 우회전

#endif /* INC_MOTOR_H_ */
