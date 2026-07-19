#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "main.h"

// 외부에서 호출하는 부저 동작 제어 함수
void Buzzer_Horn_Trigger(void);               // 경적 울림
void Buzzer_Set_Reverse_State(uint8_t state); // 후진 경고음 상태 설정 (1: 켬, 0: 끔)

// 타이머 기반 부저 상태 업데이트
void Buzzer_Update(void);

#endif /* INC_BUZZER_H_ */
