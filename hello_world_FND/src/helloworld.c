/*
 * 4자리수
 */

#include <stdio.h>
#include <xil_types.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "sleep.h"


#define FND_ID XPAR_AXI_GPIO_FND_BASEADDR
#define FND_AN_CHANNEL      1
#define FND_DISP_CHANNEL    2



int main()
{
    init_platform();

    XGpio fnd;

    u32 count = 0;
    u32 loop_cnt = 0;

    u8 fnd_data[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

    // 추가분 4자리용

    u8 anode[] = {0xE, 0xD, 0xB, 0x7};

    XGpio_Initialize(&fnd, FND_ID);

    XGpio_SetDataDirection(&fnd, FND_AN_CHANNEL, 0x0);
    XGpio_SetDataDirection(&fnd, FND_DISP_CHANNEL, 0x0);


    while (1) {
        u8 digit[4];
        digit[0] = count % 10;
        digit[1] = (count / 10) % 10;
        digit[2] = (count / 100) % 10;
        digit[3] = (count / 1000) % 10;

        for (int i =0; i < 4; i++) {
            XGpio_DiscreteWrite(&fnd, FND_AN_CHANNEL, anode[i]);
            XGpio_DiscreteWrite(&fnd, FND_DISP_CHANNEL, ~fnd_data[digit[i]]);
            usleep(1000);

        }

        loop_cnt++;
        if (loop_cnt > 50) {
            count++;
            if (count > 9999) {
                count = 0;
            }
            loop_cnt = 0;
        }

    }


    cleanup_platform();
    return 0;
}
