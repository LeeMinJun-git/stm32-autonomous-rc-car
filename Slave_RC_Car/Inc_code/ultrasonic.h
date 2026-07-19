#ifndef INC_ULTRASONIC_H_
#define INC_ULTRASONIC_H_

#include "main.h"

extern uint8_t dist_L; // 좌측 장애물까지의 거리
extern uint8_t dist_C; // 중앙 장애물까지의 거리
extern uint8_t dist_R; // 우측 장애물까지의 거리

// 센서 3개를 동시에 가동시키는 트리거 함수
void Ultrasonic_Trigger_All(void);

#endif /* INC_ULTRASONIC_H_ */
