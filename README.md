# 🚗 STM32 & FreeRTOS 기반 자율주행/수동제어 RC카 시스템

![STM32](https://img.shields.io/badge/STM32-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-000000?style=for-the-badge&logo=rtos&logoColor=white)
![C](https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=C&logoColor=black)

## 📌 프로젝트 개요
본 프로젝트는 예측 불가능한 외부 개입(수동 제어)과 내부의 자율 연산(자동 회피)이 충돌 없이 공존하는 시스템을 구축하는 것을 목표로 한 임베디드 포트폴리오입니다. 
단일 루프 방식의 병목 현상을 해결하기 위해 조종기(Master)와 구동부(Slave)를 분리한 Dual MCU 아키텍처를 설계하였으며, FreeRTOS의 선점형 스케줄링을 도입하여 완벽한 실시간 동시성을 확보했습니다.

**주요 하드웨어:** STM32F411RE (x2), HC-SR04 (x3), HC-06 Bluetooth, L298N Motor Driver
**핵심 기술:** FreeRTOS, Master-Slave Architecture, ADC-DMA, UART Interrupt, Non-Blocking Timer Control

<br/>

## 🎥 데모 영상
> https://youtube.com/shorts/L8JjzSXFRNY?si=EroMPLyY2TaXhP7T

<br/>

## 🛠 시스템 아키텍처 (Hardware & Software)

센서 노이즈와 인터럽트 충돌을 물리적으로 차단하기 위해 제어부와 구동부를 분리했습니다.
* **Master (조종기):** ADC-DMA를 이용한 조이스틱 다채널 데이터 수집 및 노이즈 필터링(Deadzone). UART를 통해 상태 변경 시에만 명령 송신.
* **Slave (RC카):** 3축 초음파 센서로 주변 공간을 인지하며, 조종기 명령 수신 및 모터/인디케이터(LED, Buzzer) 제어.
