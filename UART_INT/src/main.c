

#include <stdint.h>
#include <stdio.h>
#include <sys/_types.h>
#include "platform.h"
//#include "xil_printf.h"
#include "xparameters.h"

#include "xuartlite.h"          // UART Lite 드라이버
#include "xintc.h"              // 인터럽트 콘트롤러
#include "xil_exception.h"      // 예외처리용
#include "xstatus.h"            // XST_STATUS 등 상태값



#define UART_DEVICE_ID      XPAR_AXI_UARTLITE_0_BASEADDR
#define INTC_DEVICE_ID      XPAR_MICROBLAZE_RISCV_0_AXI_INTC_BASEADDR
#define UART_INT_IRQ_ID     XPAR_FABRIC_AXI_UARTLITE_0_INTR

// 전역변수 인스턴스
XUartLite   UartLite;               // uart 제어 구조체
XIntc       InterruptController;    // 인터럽트 콘트롤러 구조체

// 수신 버퍼        
static uint8_t RecvBuffer[1];       //static u8 RecvBuffer[1];


// 수신 콜백 함수
void RecvHandler(void *CallBackRef, unsigned int EvenData)
{
    XUartLite *UartPtr = (XUartLite *)CallBackRef;

    // EvenData <<<< 실제 수신된 데이터
    if (EvenData > 0)
    {

        // 받은데이터 그대로 다시 전송
        XUartLite_Send(UartPtr, RecvBuffer, EvenData);

        RecvBuffer[0] = NULL;


        // 다시 수신 등록
        XUartLite_Recv(UartPtr, RecvBuffer, 1);





    }
}

void SendHandler(void *CallBackRef, unsigned int EvenData)
{

}


/****** Uart 초기화 ******/
static int InitUart(void)
{
    int Status;
    XUartLite_Config *CfgPtr;

    // 구조체 가져오기
    CfgPtr = XUartLite_LookupConfig(UART_DEVICE_ID);
    if (CfgPtr == NULL) {
        return XST_FAILURE;
    }

    // 드라이버 초기화
    Status = XUartLite_CfgInitialize(&UartLite, CfgPtr, CfgPtr->RegBaseAddr);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 하드웨어 정상 확인
    Status = XUartLite_SelfTest(&UartLite);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

/****** 인터럽트 콘트롤러 초기화 ******/
static int InitIntc(void)
{
    int Status;

    // 인터럽트 콘트롤러 초기화
    Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}


/****** 인터럽트 설정 ******/
static int SetupInterrupts(void)
{
    int Status;

    // 인터럽트를 Xintc에 연결
    Status = XIntc_Connect(&InterruptController, UART_INT_IRQ_ID,
                            (XInterruptHandler)XUartLite_InterruptHandler, 
                            (void *)&UartLite);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // uart에 데이터가 들어오면 RecvHandler 실행하시오
    // 그리고 &UartLite를 같이 넘기시오
    XUartLite_SetRecvHandler(&UartLite, RecvHandler,(void *)&UartLite);
    XUartLite_SetSendHandler(&UartLite, SendHandler,(void *)&UartLite);

    // 송신은 보류

    // 인터럽트 콘트롤러 스타트
    Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }    

    // 특정 인터럽트 인에이블 -> IRQ ID
    XIntc_Enable(&InterruptController, UART_INT_IRQ_ID);

    // 내부 인터럽트 인에이블
    XUartLite_EnableInterrupt(&UartLite);

    // 마이크로 블레이즈에 연결
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                (Xil_ExceptionHandler)XIntc_InterruptHandler, 
                                (void *)&InterruptController);

    Xil_ExceptionEnable();

    return XST_SUCCESS;
}




int main()
{
    init_platform();

    int Status;

    // uart 초기화
    Status = InitUart();
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 인터럽트 콘트롤러 초기화
    Status = InitIntc();
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 인터럽트 설정
    Status = SetupInterrupts();
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 첫 수신등록
    XUartLite_Recv(&UartLite, RecvBuffer, 1);

    while (1) {
        
    }

    cleanup_platform();
    return 0;
}
















// #include "xparameters.h"      // 하드웨어 주소, ID 정의
// #include "xuartlite.h"       // UART Lite 드라이버
// #include "xintc.h"           // 인터럽트 컨트롤러 드라이버
// #include "xil_exception.h"   // 예외 처리 (인터럽트 연결용)
// #include "xstatus.h"         // XST_SUCCESS 등 상태값

// /* -------------------------------------------------------
//  * Device 정의 (2024.2 기준: BASEADDR 사용 가능)
//  * ------------------------------------------------------- */
// #define UART_DEVICE_ID      XPAR_AXI_UARTLITE_0_BASEADDR
// #define INTC_DEVICE_ID      XPAR_XINTC_0_BASEADDR
// #define UART_INT_IRQ_ID     XPAR_FABRIC_AXI_UARTLITE_0_INTR

// /* -------------------------------------------------------
//  * 드라이버 인스턴스 (전역)
//  * ------------------------------------------------------- */
// XUartLite UartLite;          // UART 제어 구조체
// XIntc     InterruptController; // 인터럽트 컨트롤러 구조체

// /* -------------------------------------------------------
//  * 수신 버퍼 (1바이트)
//  * ------------------------------------------------------- */
// static u8 RecvBuffer[1];     // UART로 들어온 데이터 저장

// /* =======================================================
//  * [핵심] 수신 콜백 함수
//  * ======================================================= */
// void RecvHandler(void *CallBackRef, unsigned int EventData)
// {
//     // CallBackRef → 우리가 넘긴 UartLite 주소
//     XUartLite *UartPtr = (XUartLite *)CallBackRef;

//     // EventData → 실제 수신된 바이트 수
//     if (EventData > 0) {

//         // 1️⃣ 받은 데이터 그대로 다시 전송 (Echo)
//         XUartLite_Send(UartPtr, RecvBuffer, EventData);

//         // 2️⃣ 핵심: 다시 수신 등록 (이거 안 하면 1번만 동작함)
//         XUartLite_Recv(UartPtr, RecvBuffer, 1);
//     }
// }

// /* -------------------------------------------------------
//  * 송신 콜백 (현재 사용 안 함)
//  * ------------------------------------------------------- */
// void SendHandler(void *CallBackRef, unsigned int EventData)
// {
//     // TX 완료 시 호출됨 (지금은 안 씀)
//     (void)CallBackRef;
//     (void)EventData;
// }

// /* =======================================================
//  * UART 초기화
//  * ======================================================= */
// static int InitUart(void)
// {
//     int Status;
//     XUartLite_Config *CfgPtr;

//     // 1️⃣ 설정 구조체 가져오기
//     CfgPtr = XUartLite_LookupConfig(UART_DEVICE_ID);
//     if (CfgPtr == NULL) {
//         return XST_FAILURE;
//     }

//     // 2️⃣ 드라이버 초기화
//     Status = XUartLite_CfgInitialize(&UartLite, CfgPtr, CfgPtr->RegBaseAddr);
//     if (Status != XST_SUCCESS) {
//         return XST_FAILURE;
//     }

//     // 3️⃣ Self Test (하드웨어 정상인지 확인)
//     Status = XUartLite_SelfTest(&UartLite);
//     if (Status != XST_SUCCESS) {
//         return XST_FAILURE;
//     }

//     return XST_SUCCESS;
// }

// /* =======================================================
//  * 인터럽트 컨트롤러 초기화
//  * ======================================================= */
// static int InitIntc(void)
// {
//     int Status;

//     // 인터럽트 컨트롤러 초기화
//     Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
//     if (Status != XST_SUCCESS) {
//         return XST_FAILURE;
//     }

//     return XST_SUCCESS;
// }

// /* =======================================================
//  * 인터럽트 설정
//  * ======================================================= */
// static int SetupInterrupts(void)
// {
//     int Status;

//     /* ---------------------------------------------------
//      * 1️⃣ UART 인터럽트를 XIntc에 연결
//      * --------------------------------------------------- */
//     Status = XIntc_Connect(&InterruptController,
//                            UART_INT_IRQ_ID,
//                            (XInterruptHandler)XUartLite_InterruptHandler,
//                            (void *)&UartLite);
//     if (Status != XST_SUCCESS) {
//         return XST_FAILURE;
//     }

//     /* ---------------------------------------------------
//      * 2️⃣ 사용자 콜백 등록
//      // "UART에서 데이터 들어오면 RecvHandler 실행해라"
// 		 // 그리고 &UartLite를 같이 넘겨라
//      * --------------------------------------------------- */
//     XUartLite_SetRecvHandler(&UartLite, RecvHandler, (void *)&UartLite);
//     XUartLite_SetSendHandler(&UartLite, SendHandler, (void *)&UartLite);

//     /* ---------------------------------------------------
//      * 3️⃣ 인터럽트 컨트롤러 시작
//      * --------------------------------------------------- */
//     Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
//     if (Status != XST_SUCCESS) {
//         return XST_FAILURE;
//     }

//     /* ---------------------------------------------------
//      * 4️⃣ 특정 인터럽트 enable (UART)
//      * --------------------------------------------------- */
//     XIntc_Enable(&InterruptController, UART_INT_IRQ_ID);

//     /* ---------------------------------------------------
//      * 5️⃣ UART 내부 인터럽트 enable
//      * --------------------------------------------------- */
//     XUartLite_EnableInterrupt(&UartLite);

//     /* ---------------------------------------------------
//      * 6️⃣ CPU(MicroBlaze)에 인터럽트 연결
//      * --------------------------------------------------- */
//     Xil_ExceptionInit();

//     Xil_ExceptionRegisterHandler(
//         XIL_EXCEPTION_ID_INT,
//         (Xil_ExceptionHandler)XIntc_InterruptHandler,
//         (void *)&InterruptController
//     );

//     Xil_ExceptionEnable();

//     return XST_SUCCESS;
// }

// /* =======================================================
//  * main
//  * ======================================================= */
// int main(void)
// {
//     int Status;

//     // 1️⃣ UART 초기화
//     Status = InitUart();
//     if (Status != XST_SUCCESS) {
//         return XST_FAILURE;
//     }

//     // 2️⃣ 인터럽트 컨트롤러 초기화
//     Status = InitIntc();
//     if (Status != XST_SUCCESS) {
//         return XST_FAILURE;
//     }

//     // 3️⃣ 인터럽트 설정
//     Status = SetupInterrupts();
//     if (Status != XST_SUCCESS) {
//         return XST_FAILURE;
//     }

//     // 4️⃣ 첫 수신 등록 (이거 없으면 인터럽트 시작 안 됨)
//     XUartLite_Recv(&UartLite, RecvBuffer, 1);

//     // 5️⃣ 메인 루프
//     while (1) {
//         // 인터럽트 기반이라 여기선 할 일 없음
//         // 필요하면 다른 작업 추가 가능
//     }

//     return 0;
// }