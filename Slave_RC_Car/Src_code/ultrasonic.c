#include "ultrasonic.h"
#include "tim.h"
#include "delay.h" // 초음파 딜레이 사용

// 초음파가 발사된 시간, 반사되어 돌아온 시간, 왕복 시간
static uint16_t start_time_L = 0, end_time_L = 0, travel_time_L = 0;
static uint16_t start_time_C = 0, end_time_C = 0, travel_time_C = 0;
static uint16_t start_time_R = 0, end_time_R = 0, travel_time_R = 0;

// 0: 발사 시점 대기 중, 1: 반사되어 돌아오길 대기 중
static uint8_t edge_state_L = 0;
static uint8_t edge_state_C = 0;
static uint8_t edge_state_R = 0;

uint8_t dist_L = 0;
uint8_t dist_C = 0;
uint8_t dist_R = 0;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM4)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {

            // 초음파가 발사되어 신호가 올라간 순간을 포착
            if(edge_state_C == 0) {
                start_time_C = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // 출발 시간 기록
                edge_state_C = 1; // 이제 돌아오는 신호를 기다리겠다고 상태 변경

                //  신호가 끝나는 순간을 잡도록 설정
                __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
            }
            // 초음파가 장애물에 맞고 돌아와 신호가 떨어진 순간 포착
            else if (edge_state_C == 1) {
                end_time_C = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // 도착 시간 기록
                __HAL_TIM_SET_COUNTER(htim, 0); // 타이머 카운터 초기화

                if(end_time_C > start_time_C) travel_time_C = end_time_C - start_time_C;
                else                          travel_time_C = (0xFFFF - start_time_C) + end_time_C;

                dist_C = travel_time_C / 58;

                // 다음 측정을 위해 초기 상태로 복구
                edge_state_C = 0;
                __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
                __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1); // 수신 채널 일시정지
            }
        }

        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            if(edge_state_L == 0) {
                start_time_L = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
                edge_state_L = 1;
                __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
            } else if (edge_state_L == 1) {
                end_time_L = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
                __HAL_TIM_SET_COUNTER(htim, 0);

                if(end_time_L > start_time_L) travel_time_L = end_time_L - start_time_L;
                else                          travel_time_L = (0xFFFF - start_time_L) + end_time_L;

                dist_L = travel_time_L / 58;

                edge_state_L = 0;
                __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
                __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC2);
            }
        }

        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
            if(edge_state_R == 0) {
                start_time_R = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
                edge_state_R = 1;
                __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
            } else if (edge_state_R == 1) {
                end_time_R = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
                __HAL_TIM_SET_COUNTER(htim, 0);

                if(end_time_R > start_time_R) travel_time_R = end_time_R - start_time_R;
                else                          travel_time_R = (0xFFFF - start_time_R) + end_time_R;

                dist_R = travel_time_R / 58;

                edge_state_R = 0;
                __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
                __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC3);
            }
        }
    }
}

// 초음파 일제 발사 함수 (SensorTask에서 주기적으로 호출)
void Ultrasonic_Trigger_All(void)
{
    // 좌측 센서
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC2); // 메아리를 듣기 위해 수신 인터럽트 ON
    HAL_Delay(15);

    // 중앙 센서
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC1);
    HAL_Delay(15);

    // 우측 센서
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_CC3);
    HAL_Delay(15);
}
