#include "led.h"

static int turn_signal_timer = 0;          // 깜빡임 주기를 계산하는 타이머
static uint8_t is_turn_signal_on = 0;      // 방향지시등의 현재 물리적 출력 상태 (ON/OFF)
static uint8_t current_turn_direction = 0; // 지시 방향 플래그 (0: 꺼짐, 1: 좌회전, 2: 우회전)

// 초기화: 전조등을 항상 켜진 상태로 유지
void LED_Init(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); // 좌측 전조등 켜기
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET); // 우측 전조등 켜기
}

// 브레이크등 점등
void LED_Brake_On(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);  // 좌측 브레이크
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); // 우측 브레이크
}

// 브레이크등 소등
void LED_Brake_Off(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
}

// 조향 상태에 따른 방향지시등 방향 기록
void LED_TurnSignal_Left(void)  { current_turn_direction = 1; }
void LED_TurnSignal_Right(void) { current_turn_direction = 2; }
void LED_TurnSignal_Off(void)   { current_turn_direction = 0; }

// 방향지시등 실시간 깜빡임 업데이트
void LED_Update(void)
{
    // 깜빡임 속도 제어
    turn_signal_timer++;
    if (turn_signal_timer >= 10)
    {
        is_turn_signal_on = !is_turn_signal_on; // 상태 뒤집기
        turn_signal_timer = 0;                  // 타이머 리셋
    }

    if (current_turn_direction == 1) // 좌회전 중일 때
    {
        // 왼쪽 핀은 타이머 상태에 맞춰 깜빡이고, 오른쪽 핀은 끈다.
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, is_turn_signal_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    }
    else if (current_turn_direction == 2) // 우회전 중일 때
    {
        // 오른쪽 핀은 타이머 상태에 맞춰 깜빡이고, 왼쪽 핀은 끈다.
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, is_turn_signal_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
    else // 평상시 (주행 또는 정지)
    {
        // 방향지시등 양쪽 모두 소등
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_RESET);
    }
}
