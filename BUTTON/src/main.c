/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xparameters.h"
#include "sleep.h"

#define GPIO_BASEADDR   XPAR_AXI_GPIO_0_BASEADDR
#define LED_CHANNEL 1
#define BTN_CHANNEL 2

#define BTN_UP      0x01
#define BTN_LEFT    0x02
#define BTN_RIGHT   0x04
#define BTN_DOWN    0x08


int main()
{
    init_platform();


    XGpio GPIO;

    XGpio_Initialize(&GPIO, GPIO_BASEADDR);

    XGpio_SetDataDirection(&GPIO, LED_CHANNEL, 0x0000);

    XGpio_SetDataDirection(&GPIO, BTN_CHANNEL, 0xFFFF);
    
    int btn_data = 0;
    int led_data = 0;

    while (1) {
        btn_data = XGpio_DiscreteRead(&GPIO, BTN_CHANNEL);

        if (btn_data & BTN_UP) {
            led_data = 0x0001;
        }
        else if (btn_data & BTN_LEFT) {
            led_data = 0x0002;
        }
        else if (btn_data & BTN_RIGHT) {
            led_data = 0x0004;
        }
        else if (btn_data & BTN_DOWN) {
            led_data = 0x0008;
        }
        else {
            led_data = 0x0000;
        }

        XGpio_DiscreteWrite(&GPIO, LED_CHANNEL, led_data);

        usleep(10000);
    }




    cleanup_platform();
    return 0;
}
