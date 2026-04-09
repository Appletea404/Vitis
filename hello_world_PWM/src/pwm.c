

#include <stdio.h>
// #include <sys/_intsup.h>
#include <xil_types.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xtmrctr.h"
#include "sleep.h"


#define TIMER_DEVICE_ID         XPAR_XTMRCTR_0_BASEADDR
#define CPU_CLK_HZ              XPAR_CPU_CORE_CLOCK_FREQ_HZ
#define PWM_FREQ_HZ             50

XTmrCtr Timer;

#define PWM_PERIOD_NS   (1000000000 / PWM_FREQ_HZ)  // 50Hz → 20,000,000 ns

static void PWM_SetDuty(int angle)
{
    u32 high_ns = 1000000 + (u32)(angle * 1000000) / 180;
    XTmrCtr_PwmDisable(&Timer);
    XTmrCtr_PwmConfigure(&Timer, PWM_PERIOD_NS, high_ns);
    XTmrCtr_PwmEnable(&Timer);
}


// static void PWM_SetDuty(u32 period, int duty)
// {
//     u32 High = (period * (u32)duty) / 100;
//     u32 reset_val;

//     if (High == 0) {
//         reset_val = 0xFFFFFFFF;
//     }
//     else {
//         reset_val = 0xFFFFFFFF - High + 1;
//     }

//     XTmrCtr_Stop(&Timer, 1);
//     XTmrCtr_SetResetValue(&Timer, 1, reset_val);
//     XTmrCtr_Start(&Timer, 1);

// }


int main()
{
    init_platform();

    
    int status;

    status = XTmrCtr_Initialize(&Timer, TIMER_DEVICE_ID);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }


    XTmrCtr_PwmConfigure(&Timer, PWM_PERIOD_NS, 0);
    XTmrCtr_PwmEnable(&Timer);


    // XTmrCtr_SetOptions(&Timer, 0, XTC_ENABLE_ALL_OPTION | XTC_AUTO_RELOAD_OPTION);
    // XTmrCtr_SetOptions(&Timer, 1, XTC_ENABLE_ALL_OPTION | XTC_AUTO_RELOAD_OPTION);


    // u32 period = CPU_CLK_HZ / PWM_FREQ_HZ;
    // XTmrCtr_SetResetValue(&Timer, 0, 0xFFFFFFFF - period + 1);
    // XTmrCtr_SetResetValue(&Timer, 1, 0xFFFFFFFF);

    // XTmrCtr_PwmEnable(&Timer);
    // XTmrCtr_Start(&Timer, 0);
    // XTmrCtr_Start(&Timer, 1);




    while (1) {


        PWM_SetDuty( 0);
        usleep(1000000);
        PWM_SetDuty( 90);
        usleep(1000000);
        PWM_SetDuty( 180);
        usleep(1000000);
        PWM_SetDuty( 270);
        usleep(1000000);
        PWM_SetDuty( 290);  //이게 최대인듯
        usleep(1000000);
        // PWM_SetDuty(period,0);
        // usleep(100000);
        // PWM_SetDuty(period,5);
        // usleep(100000);
        

        // for (int duty = 0; duty <= 10; duty++) {
        //     PWM_SetDuty(period, duty);
        //     usleep(10000);

        // }


        // for (int duty = 100; duty >= 0; duty--) {
        //     PWM_SetDuty(period, duty);
        //     usleep(10000);
        // }
    }


    cleanup_platform();
    return 0;
}
