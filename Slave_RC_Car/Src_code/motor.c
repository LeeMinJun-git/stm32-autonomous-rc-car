#include "motor.h"
#include "tim.h"

// 전진
void Motor_Forward(uint16_t speed)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);

    // 양쪽 모터에 동일한 속도(PWM) 인가
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
}

// 후진
void Motor_Backward(uint16_t speed)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
}

// 부드러운 좌회전
void Motor_Left(uint16_t left_speed, uint16_t right_speed)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, left_speed);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, right_speed);
}

// 부드러운 우회전
void Motor_Right(uint16_t left_speed, uint16_t right_speed)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, left_speed);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, right_speed);
}

// 제자리 좌회전
void Motor_Turn_Left_In_Place(uint16_t speed)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
}

// 제자리 우회전
void Motor_Turn_Right_In_Place(uint16_t speed)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
}

// 완전 정지 (Stop)
// 모든 방향 핀을 끄고 PWM 출력을 0
void Motor_Stop(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
}
