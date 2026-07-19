#ifndef INC_LED_H_
#define INC_LED_H_

#include "main.h"

// 시스템 초기화
void LED_Init(void); // 전원 인가 시 상시 켜지는 전조등 셋업

// 브레이크등 제어
void LED_Brake_On(void);  // 브레이크등 켜기
void LED_Brake_Off(void); // 브레이크등 끄기

// 방향지시등 제어
void LED_TurnSignal_Left(void);  // 좌측 방향지시등
void LED_TurnSignal_Right(void); // 우측 방향지시등
void LED_TurnSignal_Off(void);   // 방향지시등 끄기

// 타이머 기반 LED 상태 업데이트
void LED_Update(void);

#endif /* INC_LED_H_ */
