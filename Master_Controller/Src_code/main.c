/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint16_t adc_buf[4];    // [0]:Joy1_X, [1]:Joy1_Y, [2]:Joy2_X, [3]:Joy2_Y
uint16_t adc_buf[4];    // ADC 값 버퍼 (0~4095 범위)
                        // [0]: 좌측 조이스틱 Y (전/후진)
                        // [1]: 우측 조이스틱 X (좌/우)

char current_cmd = 'S'; // 현재 최종 판별된 명령어 (초기값: S - 정지)
char last_cmd = 'S';    // 직전에 전송한 명령어를 기억하는 변수
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/
	HAL_Init();
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_USART1_UART_Init(); // 블루투스 모듈과 통신하기 위한 채널

	/* USER CODE BEGIN 2 */
	// 메모리(adc_buf)에 조이스틱 아날로그 값을 자동으로 갱신
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, 4);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		// 아날로그 값 가져오기 (값의 범위: 0 ~ 4095)
		uint16_t joy1_y = adc_buf[0]; // 좌측 스틱 상하 (전진/후진 제어)
		uint16_t joy2_x = adc_buf[1]; // 우측 스틱 좌우 (좌회전/우회전 제어)

		// 물리 버튼 상태 읽기 (Pull-up 설정이므로 누르면 0이 됨)
		uint8_t btn_horn = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12); // 경적 버튼
		uint8_t btn_mode = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13); // 자동/수동 모드 전환 버튼

		// 데이터 보정 및 조작 강도 계산
		int y_move = (int)joy1_y - 2048;
		int x_move = (int)joy2_x - 2048;

		//어느 쪽으로 더 강하게 밀었는지 비교하기 위해 절댓값 산출
		int abs_y = (y_move > 0) ? y_move : -y_move;
		int abs_x = (x_move > 0) ? x_move : -x_move;

		// 스틱의 기계적 마모나 손떨림으로 인한 미세한 노이즈(오작동)를 차단
		int deadzone = 800;

		// 우선순위 및 우세 방향 판단 로직
		if (btn_mode == GPIO_PIN_RESET)
		{
			current_cmd = 'M'; // 모드 전환
			HAL_UART_Transmit(&huart1, (uint8_t*)&current_cmd, 1, 10);
			HAL_Delay(300); // 디바운싱: 꾹 누르고 있어도 여러 번 눌리지 않도록 0.3초 대기
			continue;
		}
		else if (btn_horn == GPIO_PIN_RESET)
		{
			current_cmd = 'H'; // 경적 울림
		}
		else if (abs_y <= deadzone && abs_x <= deadzone)
		{
			current_cmd = 'S'; // 정지
		}
		else if (abs_y > abs_x)
		{
			// 좌/우 조향보다 전/후진 스틱을 더 강하게 밀었을 때 (직진성 우선)
			if (y_move > 0) current_cmd = 'F'; // 위로 밀었을 때 (Forward)
			else            current_cmd = 'B'; // 아래로 당겼을 때 (Backward)
		}
		else
		{
			// 전/후진보다 좌/우 조향 스틱을 더 강하게 밀었을 때 (조향 우선)
			if (x_move > 0) current_cmd = 'L';
			else            current_cmd = 'R';
		}

		// 송신: 상태가 바뀌었을 때만 전송
		if (current_cmd != last_cmd)
		{
			HAL_UART_Transmit(&huart1, (uint8_t*)&current_cmd, 1, 10);
			last_cmd = current_cmd; // 보낸 상태 기억
		}

		// 버튼 로직(M, H) 등 연속 입력이 필요한 경우는 예외적으로 추가 처리
		else if (current_cmd == 'M' || current_cmd == 'H')
		{
			HAL_UART_Transmit(&huart1, (uint8_t*)&current_cmd, 1, 10);
		}

		// 딜레이 유지
		HAL_Delay(50);

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 100;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

void Error_Handler(void)
{
	__disable_irq();
	while (1)
	{
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
