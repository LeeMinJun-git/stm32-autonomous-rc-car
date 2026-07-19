#include "buzzer.h"

static int horn_time_left = 0;          // 경적 소리가 유지될 남은 시간
static int reverse_sound_timer = 0;     // 후진음 발생 주기를 재는 타이머
static uint8_t is_buzzer_on = 0;        // 실제 부저 핀의 현재 물리적 출력 상태 (ON/OFF)
static uint8_t is_reversing = 0;        // 차량이 후진 중인지 여부를 저장하는 플래그

// 경적 발생 트리거
void Buzzer_Horn_Trigger(void)
{
    horn_time_left = 50;
}

// 차량 후진 상태 플래그 설정
void Buzzer_Set_Reverse_State(uint8_t state)
{
    is_reversing = state;
}

// 부저 상태 실시간 업데이트
void Buzzer_Update(void)
{
    // 경적
    if (horn_time_left > 0)
    {
        if (horn_time_left % 2 == 0) {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
        }

        horn_time_left--;          // 시간 차감
        reverse_sound_timer = 0;   // 경적이 울리는 동안 후진음 타이머는 꼬이지 않게 초기화
    }
    // 후진 경고음 (경적이 울리지 않을 때만 동작)
    else if (is_reversing == 1)
    {
        reverse_sound_timer++;

        if (reverse_sound_timer >= 30)
        {
            is_buzzer_on = !is_buzzer_on; // ON/OFF 상태 반전
            reverse_sound_timer = 0;      // 타이머 리셋
        }

        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, is_buzzer_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
    // 평상시 무음 상태
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); // 부저 끄기
        reverse_sound_timer = 0;                               // 타이머 리셋
        is_buzzer_on = 0;                                      // 상태 초기화
    }
}
