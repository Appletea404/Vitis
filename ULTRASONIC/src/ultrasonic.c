#include <stdio.h>
#include <xgpio_l.h>        // XGpio 저수준 레지스터 접근 (XGPIO_IR_CH1_MASK 등)
#include <xil_types.h>      // u8, u16, u32 등 Xilinx 기본 타입
#include <xstatus.h>        // XST_SUCCESS, XST_FAILURE 상태 코드
#include "platform.h"       // init_platform / cleanup_platform
#include "xil_printf.h"     // 경량 UART 출력
#include "xparameters.h"    // Vivado 생성 하드웨어 파라미터
#include "xgpio.h"          // AXI GPIO 드라이버
#include "xintc.h"          // AXI Interrupt Controller 드라이버
#include "xtmrctr.h"        // AXI Timer 드라이버
#include "xil_exception.h"  // MicroBlaze 예외 핸들러
#include "sleep.h"          // usleep

/* ── 디바이스 ID (베이스 주소) ─────────────────────────────────────────────
 * Vivado Block Design에서 할당된 주소가 xparameters.h에 자동 생성됨        */
#define GPIO_TRIG_ID        XPAR_AXI_GPIO_0_BASEADDR                    // 트리거 출력용 GPIO
#define GPIO_ECHO_ID        XPAR_AXI_GPIO_1_BASEADDR                    // 에코 입력용 GPIO
#define TIMER_ID            XPAR_AXI_TIMER_0_BASEADDR                   // 경과 시간 측정용 타이머
#define INTC_ID             XPAR_MICROBLAZE_RISCV_0_AXI_INTC_BASEADDR   // 인터럽트 컨트롤러
#define GPIO_ECHO_INT_ID    XPAR_FABRIC_AXI_GPIO_1_INTR                 // INTC에 연결된 에코 GPIO 인터럽트 ID

/* ── GPIO 채널 번호 ─────────────────────────────────────────────────────── */
#define TRIG_CHANNEL        1   // 트리거 출력 채널
#define ECHO_CHANNEL        1   // 에코 입력 채널

/* ── 타이밍 설정 ────────────────────────────────────────────────────────── */
#define TIMER_CLOCK_HZ      XPAR_CPU_CORE_CLOCK_FREQ_HZ    // 100MHz = 10ns (1 clock)
#define TRIG_PULSE_US       10                              // HC-SR04 트리거 최소 펄스: 10us
#define MEASURE_TIMEOUT_US  50000                           // 측정 타임아웃: 50ms
#define LOOP_INTERVAL_US    1000000UL                       // 측정 주기: 1초마다 측정

/* ── 전역 드라이버 인스턴스 ─────────────────────────────────────────────── */
XGpio   GpioTrig;   // 트리거 핀 GPIO 인스턴스
XGpio   GpioEcho;   // 에코 핀 GPIO 인스턴스
XIntc   Intc;       // 인터럽트 컨트롤러 인스턴스
XTmrCtr Timer;      // 타이머 인스턴스

/* ── ISR 공유 변수 ──────────────────────────────────────────────────────── 
 * ISR과 메인 루프가 공유하므로 volatile 필수                               */
volatile u32 end_time   = 0;    // 에코 하강엣지 시점의 타이머 카운터 값
volatile int done       = 0;    // 측정 완료 플래그 (1 = 결과 유효)
volatile int measureing = 0;    // 측정 진행 중 플래그 (중복 트리거 방지)

/******************************************************************************
 * Echo_Handler - 에코 핀 GPIO 인터럽트 서비스 루틴
 *
 * HC-SR04 에코 핀의 상태 변화(상승/하강 엣지)마다 호출된다.
 *
 * 상승 엣지 (echo = 1): 초음파 발사 시작 → 타이머 카운트 시작
 * 하강 엣지 (echo = 0): 초음파 수신 완료 → 타이머 값 저장
 *
 * @param CallBackRef   XIntc_Connect() 시 전달한 포인터 (여기선 &GpioEcho)
 ******************************************************************************/
void Echo_Handler(void *CallBackRef)
{
    u32 echo_val;

    // GPIO 인터럽트 플래그 클리어 (클리어 안 하면 ISR이 무한 재진입)
    XGpio_InterruptClear(&GpioEcho, XGPIO_IR_CH1_MASK);

    // 현재 에코 핀 상태 읽기
    echo_val = XGpio_DiscreteRead(&GpioEcho, ECHO_CHANNEL);

    if (echo_val & 0x01) {
        // 상승 엣지: 에코 신호 HIGH → 초음파 송신 시작
        if (!measureing) {              // 중복 측정 방지
            measureing = 1;
            XTmrCtr_Reset(&Timer, 0);  // 타이머 카운터 초기화
            XTmrCtr_Start(&Timer, 0);  // 카운트 시작
        }
    }
    else {
        // 하강 엣지: 에코 신호 LOW → 초음파 수신 완료
        if (measureing) {
            end_time = XTmrCtr_GetValue(&Timer, 0); // 경과 틱 수 저장
            XTmrCtr_Stop(&Timer, 0);                // 타이머 정지
            done = 1;                               // 결과 유효 표시
            measureing = 0;                         // 측정 종료
        }
    }
}

/******************************************************************************
 * trig_pulse - HC-SR04 트리거 펄스 발생
 *
 * 10us HIGH 펄스를 트리거 핀에 출력하여 초음파 송신을 시작시킨다.
 * HC-SR04 데이터시트: 최소 10us HIGH 펄스 필요
 ******************************************************************************/
void trig_pulse(void)
{
    XGpio_DiscreteWrite(&GpioTrig, TRIG_CHANNEL, 1); // 트리거 HIGH
    usleep(TRIG_PULSE_US);                            // 10us 대기
    XGpio_DiscreteWrite(&GpioTrig, TRIG_CHANNEL, 0); // 트리거 LOW
}

/******************************************************************************
 * Interrupt_Init - AXI Interrupt Controller 초기화
 *
 * 에코 GPIO 인터럽트를 INTC에 연결하고 MicroBlaze 예외 핸들러에 등록한다.
 *
 * @return XST_SUCCESS / XST_FAILURE
 ******************************************************************************/
int Interrupt_Init(void)
{
    int status;

    // INTC 초기화
    status = XIntc_Initialize(&Intc, INTC_ID);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 에코 GPIO 인터럽트를 INTC에 연결 + 핸들러 등록
    status = XIntc_Connect(&Intc, GPIO_ECHO_INT_ID,
                           (XInterruptHandler)Echo_Handler, &GpioEcho);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // INTC 시작 (실제 하드웨어 인터럽트 모드)
    status = XIntc_Start(&Intc, XIN_REAL_MODE);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 에코 GPIO 인터럽트 채널 활성화 (마스크 해제)
    XIntc_Enable(&Intc, GPIO_ECHO_INT_ID);

    // MicroBlaze 예외 핸들러에 INTC 등록 및 전역 인터럽트 활성화
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler)XIntc_InterruptHandler, &Intc);
    Xil_ExceptionEnable();

    return XST_SUCCESS;
}

int main()
{
    init_platform();

    u32 time_us;        // 에코 왕복 시간 (us)
    u32 distance_cm;    // 계산된 거리 (cm)
    u32 time_cnt;       // 타임아웃 카운터

    // 트리거, 에코 GPIO 초기화
    XGpio_Initialize(&GpioTrig, GPIO_TRIG_ID);
    XGpio_Initialize(&GpioEcho, GPIO_ECHO_ID);

    // 트리거: 출력(0x0), 에코: 입력(0x1)
    XGpio_SetDataDirection(&GpioTrig, TRIG_CHANNEL, 0x0);
    XGpio_SetDataDirection(&GpioEcho, ECHO_CHANNEL, 0x1);

    // GPIO 인터럽트 활성화
    XGpio_InterruptEnable(&GpioEcho, XGPIO_IR_CH1_MASK); // 채널 1 인터럽트 활성화
    XGpio_InterruptGlobalEnable(&GpioEcho);               // GPIO 전역 인터럽트 활성화

    // 타이머 초기화 (옵션 0 = 기본 설정, 인터럽트/자동재시작 없음)
    XTmrCtr_Initialize(&Timer, TIMER_ID);
    XTmrCtr_SetOptions(&Timer, 0, 0);

    // 인터럽트 컨트롤러 초기화
    Interrupt_Init();

    print("UltraSonic Ready\r\n");

    while (1) {
        done = 0;       // 측정 완료 플래그 초기화
        measureing = 0; // 측정 진행 중 플래그 초기화

        trig_pulse();   // 초음파 트리거 펄스 발생

        // 에코 수신 대기 (최대 MEASURE_TIMEOUT_US = 50ms)
        time_cnt = 0;
        while (!done && time_cnt < MEASURE_TIMEOUT_US) {
            usleep(1);
            time_cnt++;
        }

        // 타임아웃: 에코 신호 미수신 (장애물 없음 또는 범위 초과)
        if (!done) {
            print("TimeOut\r\n");
            usleep(LOOP_INTERVAL_US);
            continue;
        }

        // 100MHz 클럭 기준 틱 → us 변환 (1us = 100틱)
        time_us = end_time / 100;

        // 노이즈 조금 제거
        // HC-SR04 유효 범위: 1.5cm 이하, 400cm 이상이면 Invalid
        if (time_us < 100 || time_us > 23000) {
            printf("Invalid : %lu us\r\n", (unsigned long)time_us);
            usleep(LOOP_INTERVAL_US);
            continue;
        }

        // 거리 계산: 58로 나누는 것과 같다
        // distance_cm = time_us / 58
        // = (time_us * 1715) / 100000  (정수 연산으로 대체)
        distance_cm = (time_us * 1715) / 100000;

        printf("Distance : %lu cm (time : %lu us)\r\n",
               (unsigned long)distance_cm, (unsigned long)time_us);
        usleep(LOOP_INTERVAL_US); // 1초마다 측정
    }

    cleanup_platform();
    return 0;
}