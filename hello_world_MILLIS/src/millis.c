

#include <stdio.h>
#include <xil_types.h>      // Xilinx 기본 타입 정의 (u8, u16, u32 등)
#include <xstatus.h>        // XST_SUCCESS, XST_FAILURE 등 상태 코드 정의
#include "platform.h"       // init_platform / cleanup_platform (캐시, UART 초기화)
#include "xil_printf.h"     // 경량 printf (UART 출력용)
#include "xparameters.h"    // Vivado에서 생성된 하드웨어 파라미터 (베이스 주소, 인터럽트 ID 등)
#include "xgpio.h"          // AXI GPIO 드라이버
#include "xil_exception.h"  // 예외(Exception) 핸들러 등록 API
#include "xtmrctr.h"        // AXI Timer 드라이버
#include "xintc.h"          // AXI Interrupt Controller 드라이버

/* ── 디바이스 ID 정의 ───────────────────────────────────────────────────────
 * xparameters.h에 Vivado가 자동으로 생성한 매크로를 사용한다.
 * 실제 값은 하드웨어 설계(Block Design)에 따라 달라진다.              */
#define GPIO_DEVICE_ID      XPAR_XGPIO_0_BASEADDR   // GPIO 베이스 주소
#define TIMER_DEVICE_ID     XPAR_XTMRCTR_0_BASEADDR // Timer 베이스 주소
#define INTC_DEVICE_ID      XPAR_XINTC_0_BASEADDR   // Interrupt Controller 베이스 주소

/* ── 인터럽트 ID ─────────────────────────────────────────────────────────── */
#define TIMER_INTR_ID       XPAR_FABRIC_XTMRCTR_0_INTR  // INTC에 연결된 타이머 인터럽트 번호

/* ── 타이머 주기 계산 ───────────────────────────────────────────────────────
 * AXI Timer는 32비트 카운터로 동작하며,
 * 설정된 Reset Value에서 카운트업하여 오버플로우 시 인터럽트를 발생시킨다.
 *
 * TIMER_RESET_VAL 계산 원리:
 *   - 카운터는 Reset Value → 0xFFFFFFFF 까지 카운트업 후 오버플로우
 *   - 원하는 카운트 수 = CPU_CLK_HZ * TOGGLE_PERIOD_S
 *   - Reset Value = 0xFFFFFFFF - (원하는 카운트 수) + 1
 *
 * 예) 100MHz 클럭, 0.5초 주기:
 *   원하는 카운트 수 = 100,000,000 * 0.5 = 50,000,000
 *   Reset Value     = 0xFFFFFFFF - 50,000,000 + 1 = 0xFD050F81          */
#define CPU_CLK_HZ          100000000UL             // MicroBlaze 클럭: 100MHz
#define TIMER_PERIOD_MS     1                       // 1ms                    

#define TIMER_RESET_VAL     (0xFFFFFFFF - (u32)(CPU_CLK_HZ / 1000 * TIMER_PERIOD_MS) + 1UL)

/* ── 전역 드라이버 인스턴스 ─────────────────────────────────────────────────
 * Xilinx 드라이버는 각 IP마다 전용 구조체(인스턴스)를 사용하여 상태를 관리한다. */
XGpio   Gpio;   // AXI GPIO 인스턴스
XTmrCtr Timer;  // AXI Timer 인스턴스
XIntc   Intc;   // AXI Interrupt Controller 인스턴스

/* ── LED 상태 변수 ───────────────────────────────────────────────────────────
 * ISR(인터럽트 서비스 루틴)과 메인 루프가 공유하는 변수.
 * volatile: 컴파일러가 최적화로 인해 레지스터 캐시를 사용하지 않도록 강제.
 * 인터럽트로 변경될 수 있는 변수에는 반드시 volatile을 붙여야 한다.        */
volatile u32 led_state = 0;

volatile u32 millis = 0;

u32 millis_get(void)
{
    return millis;
}

void TimerISR(void *CallBackRef, u8 TmCtrNumber)
{
    millis++;
}

int GPIO_Init(void)
{
    int status;

    // GPIO 드라이버 초기화: 내부 구조체를 설정하고 IP 레지스터를 초기화
    status = XGpio_Initialize(&Gpio, GPIO_DEVICE_ID);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 채널 1의 데이터 방향 설정: 0x00 = 전체 핀 출력
    // (비트가 0이면 출력, 1이면 입력)
    XGpio_SetDataDirection(&Gpio, 1, 0x00);

    // 초기 LED 상태를 0(소등)으로 설정
    XGpio_DiscreteWrite(&Gpio, 1, 0);

    return XST_SUCCESS;
}

int Timer_Init(void)
{
    int status;

    // 타이머 드라이버 초기화
    status = XTmrCtr_Initialize(&Timer, TIMER_DEVICE_ID);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 인터럽트 발생 시 호출될 사용자 핸들러 등록
    // XTmrCtr_InterruptHandler → (내부) → TimerISR 순으로 호출됨
    XTmrCtr_SetHandler(&Timer, TimerISR, &Timer);

    // 타이머 동작 옵션 설정 (타이머 번호 0)
    // XTC_INT_MODE_OPTION    : 오버플로우 시 인터럽트 발생
    // XTC_AUTO_RELOAD_OPTION : 오버플로우 후 Reset Value로 자동 복귀 (반복)
    XTmrCtr_SetOptions(&Timer, 0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);

    // 타이머 시작값(Reset Value) 설정
    // 카운터는 이 값에서 시작하여 0xFFFFFFFF까지 증가 후 오버플로우
    XTmrCtr_SetResetValue(&Timer, 0, TIMER_RESET_VAL);

    return XST_SUCCESS;
}

int Interrupt_Init(void)
{
    int status;

    // INTC 드라이버 초기화
    status = XIntc_Initialize(&Intc, INTC_DEVICE_ID);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 타이머 인터럽트 ID를 INTC에 연결하고 핸들러 등록
    // XTmrCtr_InterruptHandler: Xilinx 타이머 드라이버의 공용 ISR
    //   → 내부에서 타이머 번호를 판별하여 등록된 TimerISR을 호출
    status = XIntc_Connect(&Intc, TIMER_INTR_ID,
                           (XInterruptHandler)XTmrCtr_InterruptHandler, &Timer);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // INTC 시작: XIN_REAL_MODE = 실제 하드웨어 인터럽트 모드 (시뮬레이션 모드와 구분)
    status = XIntc_Start(&Intc, XIN_REAL_MODE);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // INTC에서 타이머 인터럽트 채널을 활성화 (마스크 해제)
    XIntc_Enable(&Intc, TIMER_INTR_ID);

    // MicroBlaze 예외 핸들러 초기화 및 INTC 연결
    Xil_ExceptionInit(); // 예외 벡터 테이블 초기화

    // 외부 인터럽트(XIL_EXCEPTION_ID_INT)가 발생하면
    // XIntc_InterruptHandler가 호출되도록 등록
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler)XIntc_InterruptHandler, &Intc);

    // MicroBlaze의 전역 인터럽트 활성화 (MSR의 IE 비트 set)
    Xil_ExceptionEnable();

    return XST_SUCCESS;
}

int main()
{
    init_platform();

    int status;
    xil_printf("Timer Interrupt LED Toggle \n\r");

    /* GPIO 초기화 */
    status = GPIO_Init();
    if (status != XST_SUCCESS) {
        xil_printf("GPIO Init Failed\n\r");
        return -1;
    }
    xil_printf("GPIO Init Success\r\n");

    /* 타이머 초기화 */
    status = Timer_Init();
    if (status != XST_SUCCESS) {
        xil_printf("Timer Init Failed\n\r");
        return -1;
    }
    xil_printf("Timer Init Success\r\n");

    /* 인터럽트 컨트롤러 초기화 */
    status = Interrupt_Init();
    if (status != XST_SUCCESS) {
        xil_printf("Interrupt Init Failed\n\r");
        return -1;
    }
    xil_printf("Interrupt Init Success\r\n");

    /* 타이머 카운터 0번 시작 → 0.5초마다 TimerISR 호출 시작 */
    XTmrCtr_Start(&Timer, 0);

    u32 prev1 = 0;
    u32 prev2 = 0;

    u32 led0 = 0;
    u32 led1 = 0;



    while (1) {
    
        u32 curr = millis_get();

        if (curr - prev1 >= 500) {
             prev1 = curr;
             led0 ^= 1;
        }
        if (curr - prev2 >= 1000) {
             prev2 = curr;
             led1 ^= 1;
        }

        u32 output = (led1 << 1) | led0;

        XGpio_DiscreteWrite(&Gpio, 1, output);

    }


    cleanup_platform();
    return 0;
}
