# 🚗 STM32 & FreeRTOS 기반 자율주행/수동제어 RC카 시스템

![STM32](https://img.shields.io/badge/STM32-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-000000?style=for-the-badge&logo=rtos&logoColor=white)
![C](https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=C&logoColor=black)

## 📌 프로젝트 개요
본 프로젝트는 예측 불가능한 외부 개입(수동 제어)과 내부의 자율 연산(자동 회피)이 충돌 없이 공존하는 시스템을 구축하는 것을 목표로 한 임베디드 포트폴리오입니다. 
단일 루프 방식의 병목 현상을 해결하기 위해 조종기(Master)와 구동부(Slave)를 분리한 Dual MCU 아키텍처를 설계하였으며, FreeRTOS의 선점형 스케줄링을 도입하여 완벽한 실시간 동시성을 확보했습니다.

**주요 하드웨어:** STM32F411RE (x2), HC-SR04 (x3), HC-06 Bluetooth, L298N Motor Driver
<br/>
**핵심 기술:** FreeRTOS, Master-Slave Architecture, ADC-DMA, UART Interrupt, Non-Blocking Timer Control

<br/>

## 🎥 데모 영상
> https://youtube.com/shorts/L8JjzSXFRNY?si=EroMPLyY2TaXhP7T

<br/>

## 🛠 시스템 아키텍처 (Hardware & Software Architecture)

본 프로젝트는 센서 노이즈와 통신 인터럽트 충돌을 물리적으로 차단하고, 각 MCU가 본연의 연산에 집중할 수 있도록 분산 제어 환경을 설계했습니다.

### 1. 하드웨어 리소스 분배 (Dual MCU)
* **Master Controller (조종기):** 
  * `ADC-DMA`: 조이스틱의 X, Y축 아날로그 값을 CPU 개입 없이 백그라운드에서 연속적으로 수집합니다.
  * `UART (Tx)`: 연산된 최종 조향 명령을 블루투스(HC-06)를 통해 Slave로 송신합니다.
* **Slave RC Car (구동부):**
  * `TIM (Timer)`: 3개의 초음파 센서(HC-SR04) 트리거 및 에코 펄스 측정(TIM3, TIM11)과 모터 PWM 속도 제어(TIM4)를 전담합니다.
  * `UART (Rx/Interrupt)`: 수신 인터럽트를 통해 외부 개입(조종 명령)을 즉각적으로 낚아채어 전역 변수를 갱신합니다.
  * `GPIO`: 좌/우 방향지시등, 후미등(브레이크등), 부저(Buzzer) 등 차량 인디케이터를 제어합니다.

### 2. FreeRTOS 기반 태스크 스케줄링
자율주행 중 '장애물 감지 및 충돌 방지'라는 최우선 목표(Fail-Safe)를 달성하기 위해, 태스크의 우선순위를 철저하게 분리한 선점형(Preemptive) OS 환경을 구축했습니다.

* **SensorTask (우선순위: AboveNormal)**
  * 30ms 주기로 깨어나 3축 초음파 센서의 거리를 갱신합니다.
  * 구동부 연산보다 높은 우선순위를 가져, 주행 중 어떤 상황에서도 주변 공간 인지에 사각지대가 발생하지 않도록 보장합니다.
* **DriveTask (우선순위: Normal)**
  * `SensorTask`가 수집한 거리 데이터와 `UART ISR`이 수신한 블루투스 명령을 바탕으로 실제 모터 구동과 인디케이터 점멸 로직을 수행합니다.

<br/>

## ⚙️ 주요 기능 및 동작 논리 (Core Features)

### 1. 끊김 없는 듀얼 주행 모드 (Dual Driving Mode)
조종기의 'M(Mode)' 버튼 클릭 한 번으로 수동 조종 모드와 자율주행 모드가 실시간으로 스위칭됩니다. 통신 수신은 인터럽트로 처리되어, 자율주행 연산 중에도 즉각적으로 사용자의 모드 전환 명령을 수용합니다.

### 2. 3축 센서 기반 지능형 자율 회피 (Autonomous Evasion)
단순히 전방 장애물만 멈추는 것을 넘어, 능동적으로 경로를 탐색하고 맹점(Blind-spot)을 방어합니다.
* **경로 탐색 로직:** 전방에 장애물 감지(28cm 이내) 시 즉시 브레이크를 작동한 후, 좌측과 우측의 잔여 공간을 수학적으로 비교하여 더 넓은 개활지 쪽으로 회전 방향을 스스로 결정합니다.
* **측면 사각지대 방어:** 전면이 비어있더라도 측면(6cm 이하)으로 벽이나 장애물이 근접하면, 즉시 반대 방향으로 스핀(Spin) 회피 기동을 수행하여 차체 측면 충돌을 방지합니다.
* **결측치 전처리:** 초음파 반사파 유실로 인한 거리 0cm 수신 시, 이를 즉각 안전거리(30cm)로 강제 보정하여 센서 오작동으로 인한 급발진을 차단합니다.

### 3. 정밀 수동 조향 및 노이즈 필터링 (Precision Manual Control)
조이스틱의 아날로그 조작을 정교한 차량 움직임으로 변환합니다.
* **벡터 기반 의도 추출:** 조이스틱의 X축과 Y축 변화량(절댓값)을 비교하여 사용자가 '직진/후진'을 원하는지, '좌/우 조향'을 원하는지 수학적으로 우위를 판별합니다.
* **소프트웨어 데드존(Deadzone):** 조이스틱 탄성 유격으로 인한 미세한 흔들림 노이즈를 데드존 알고리즘으로 차단하여 정차 시 차량의 떨림을 완벽히 방지합니다.
* **상태 기반 송신 최적화:** 무조건적인 데이터 송신을 배제하고, 조작 상태(`current_cmd`)가 이전 상태(`last_cmd`)와 달라졌을 때만 패킷을 송신하여 통신 트래픽 부하를 최소화했습니다.

### 4. 실제 차량 수준의 동시성 인디케이터 (Real-time Concurrency)
차량이 회피 기동(제자리 회전)을 수행하는 긴 시간 동안에도 방향지시등과 후진음(Buzzer)이 멈추지 않고 작동합니다.
* 단순한 RTOS Blocking 딜레이(`osDelay`)를 사용하지 않고, 하드웨어 절대 시간(`HAL_GetTick()`)을 참조하는 **Active_Delay(Non-Blocking Timer)** 루프를 독자적으로 구현했습니다. 
* 이를 통해 목표 회전 각도를 맞추기 위해 대기하는 동안에도 10ms 단위로 LED와 부저 상태를 병렬로 갱신하여, 차량의 구동 로직과 시청각 알림 로직의 완벽한 멀티태스킹을 달성했습니다.
