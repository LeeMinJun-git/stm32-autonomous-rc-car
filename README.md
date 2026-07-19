# 🚗 STM32 & FreeRTOS 기반 자율주행/수동제어 RC카 시스템

![STM32](https://img.shields.io/badge/STM32-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-000000?style=for-the-badge&logo=rtos&logoColor=white)
![C](https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=C&logoColor=black)

## 📌 프로젝트 개요
본 프로젝트는 **"예측 불가능한 외부 개입(수동 제어)과 내부의 자율 연산(자동 회피)이 충돌 없이 공존하는 시스템"**을 구축하는 것을 목표로 한 임베디드 포트폴리오입니다. 
단일 루프(Super-loop) 방식의 병목 현상을 해결하기 위해 조종기(Master)와 구동부(Slave)를 분리한 **Dual MCU 아키텍처**를 설계하였으며, FreeRTOS의 선점형 스케줄링을 도입하여 완벽한 실시간 동시성(Concurrency)을 확보했습니다.

* **개발 기간:** 202X.XX ~ 202X.XX (약 X주)
* **주요 하드웨어:** STM32F411RE (x2), HC-SR04 (x3), HC-06 Bluetooth, L298N Motor Driver
* **핵심 기술:** FreeRTOS, Master-Slave Architecture, ADC-DMA, UART Interrupt, Non-Blocking Timer Control

<br/>

## 🎥 데모 영상
> *이곳에 자율주행 및 수동제어 동작을 보여주는 GIF 이미지나 YouTube 링크를 삽입하세요.*
> * `![Demo](이미지_링크.gif)` 또는 `[유튜브 시연 영상 보러가기](링크)`

<br/>

## 🛠 시스템 아키텍처 (Hardware & Software)

### 1. Master-Slave 하드웨어 분리 설계
센서 노이즈와 인터럽트 충돌을 물리적으로 차단하기 위해 제어부와 구동부를 분리했습니다.
* **Master (조종기):** ADC-DMA를 이용한 조이스틱 다채널 데이터 수집 및 노이즈 필터링(Deadzone). UART를 통해 상태 변경 시에만 명령 송신.
* **Slave (RC카):** 3축 초음파 센서로 주변 공간을 인지하며, 조종기 명령 수신 및 모터/인디케이터(LED, Buzzer) 제어.

### 2. FreeRTOS 기반 선점형 스케줄링 (Preemptive Scheduling)
자율주행 중 충돌 방지라는 최우선 목표(Fail-Safe)를 달성하기 위해 태스크의 우선순위를 비대칭적으로 설계했습니다.

```mermaid
sequenceDiagram
    participant CPU as MCU (Core)
    participant ST as SensorTask (AboveNormal)
    participant DT as DriveTask (Normal)

    Note over CPU, DT: DriveTask 구동 중
    CPU ->> DT: 모터 제어 및 연산
    ST -) CPU: 30ms 주기 Wake up (선점)
    CPU -->> DT: DriveTask 강제 일시 정지
    CPU ->> ST: SensorTask 실행 및 장애물 감지
    ST -) CPU: osDelay(30) 진입 (Block)
    CPU ->> DT: 중단되었던 모터 연산 즉시 재개
