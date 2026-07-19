/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"
#include "buzzer.h"
#include "led.h"
#include "ultrasonic.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern volatile char bt_cmd;
extern volatile int system_mode;
/* USER CODE END Variables */
/* Definitions for DriveTask */
osThreadId_t DriveTaskHandle;
const osThreadAttr_t DriveTask_attributes = {
  .name = "DriveTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SensorTask */
osThreadId_t SensorTaskHandle;
const osThreadAttr_t SensorTask_attributes = {
  .name = "SensorTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Active_Delay(uint32_t delay_ms)
{
		// 현재의 시간을 시작 시간으로 기록
	    uint32_t start_time = HAL_GetTick();

	    // 스톱워치 시간이 원하는 목표 시간을 넘길 때까지 무한 반복
	    while ((HAL_GetTick() - start_time) < delay_ms)
	    {
	        LED_Update();     // LED 갱신
	        Buzzer_Update();  // 부저 갱신

	        osDelay(10);
	    }
}
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of DriveTask */
  DriveTaskHandle = osThreadNew(StartDefaultTask, NULL, &DriveTask_attributes);

  /* creation of SensorTask */
  SensorTaskHandle = osThreadNew(StartTask02, NULL, &SensorTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the DriveTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */

  LED_Init(); // 전조등 켜기

  /* Infinite loop */
  for(;;)
  {
      if (system_mode == 0)
      {
          // 수동 모드: 조이스틱 & 스마트폰 제어
          switch(bt_cmd)
          {
              case 'H': // 경적 버튼
                  Buzzer_Horn_Trigger();
                  bt_cmd = 'S';
                  break;

              case 'F': // 전진
                  LED_Brake_Off();
                  LED_TurnSignal_Off();
                  Buzzer_Set_Reverse_State(0);
                  Motor_Forward(600);
                  break;

              case 'B': // 후진
                  LED_Brake_Off();
                  LED_TurnSignal_Off();
                  Buzzer_Set_Reverse_State(1); // 후진음 ON
                  Motor_Backward(600);
                  break;

              case 'L': // 좌회전
                  LED_Brake_Off();
                  LED_TurnSignal_Left();       // 좌회전 깜빡이 ON
                  Buzzer_Set_Reverse_State(0);
                  Motor_Left(0, 600);
                  break;

              case 'R': // 우회전
                  LED_Brake_Off();
                  LED_TurnSignal_Right();      // 우회전 깜빡이 ON
                  Buzzer_Set_Reverse_State(0);
                  Motor_Right(600, 0);
                  break;

              case 'S':
              default:
                  LED_Brake_On(); 				// 브레이크등 점등
                  LED_TurnSignal_Off();
                  Buzzer_Set_Reverse_State(0);
                  Motor_Stop();
                  break;
          }
      }
      else if (system_mode == 1)
      {
          // 자동 모드: 자율주행

    	  // 센서 측정값이 0으로 튀는 노이즈 발생 시 안전거리(30)로 보정
          if (dist_C == 0) dist_C = 30;
          if (dist_L == 0) dist_L = 30;
          if (dist_R == 0) dist_R = 30;

          // 중앙 장애물 감지
          if (dist_C <= 28)
          {
              LED_Brake_On();
              Motor_Stop();
              osDelay(150);

              LED_Brake_Off();

              // 양옆 공간을 비교하여 더 넓은 쪽으로 제자리 회전
              if (dist_L > dist_R) {
                  LED_TurnSignal_Left();
                  Motor_Turn_Left_In_Place(400);
                  Active_Delay(575);
              } else {
                  LED_TurnSignal_Right();
                  Motor_Turn_Right_In_Place(400);
                  Active_Delay(575);
              }
              LED_TurnSignal_Off();
          }
          // 왼쪽 사각지대 근접 감지
          else if (dist_L <= 6)
          {
              LED_Brake_On();
              Motor_Stop();
              Active_Delay(50);

              LED_Brake_Off();
              LED_TurnSignal_Right();
              Motor_Turn_Right_In_Place(400);
              Active_Delay(100);

              LED_TurnSignal_Off();
          }
          // 오른쪽 사각지대 근접 감지
          else if (dist_R <= 6)
          {
              LED_Brake_On();
              Motor_Stop();
              Active_Delay(50);

              LED_Brake_Off();
              LED_TurnSignal_Left();
              Motor_Turn_Left_In_Place(400);
              Active_Delay(100);

              LED_TurnSignal_Off();
          }
          else
          {
              // 전방 및 측면에 장애물이 없을 때
              LED_Brake_Off();
              LED_TurnSignal_Off();
              Motor_Forward(350);
          }
      }

      LED_Update();     // 방향지시등 깜빡임 처리
      Buzzer_Update();  // 경적 및 후진음 처리

      osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
 * @brief Function implementing the SensorTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  for(;;)
  {
      if (system_mode == 1) // 자동 모드일 때만 초음파 센서 사용
      {
          Ultrasonic_Trigger_All(); // 함수 호출
      }
      osDelay(30);
  }
  /* USER CODE END StartTask02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

