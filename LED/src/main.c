#include "def.h"
#include "led.h"
#include <sleep.h>





int main()
{
    init_platform();

    XGpio led_gpio;         // 사용자 정의형 변수 선언 

    LED_Init(&led_gpio);


    while (1) {
        // LED_ALLOn(&led_gpio);
        // usleep(100000);
        // LED_ALLOff(&led_gpio);
        // usleep(100000);

        // LED_MoveLeft(&led_gpio, 100000);
        // LED_MoveRight(&led_gpio, 100000);
        // LED_KnightRider(&led_gpio, 100000);
        LED_CenterOut(&led_gpio, 100000);
          
        
    }
    cleanup_platform();
    return 0;
}















// #include <stdio.h>
// #include "platform.h"
// #include "xil_printf.h"
// #include "sleep.h"
// #include "xparameters.h"
// #include "xgpio.h"

// #define GPIO_BASEADDR   XPAR_AXI_GPIO_0_BASEADDR 
// #define LED_CHANNEL     1                           // GPIO 채널 1 사용 



// int main()
// {
//     init_platform();

//     XGpio led_gpio;         // 사용자 정의형 변수 선언 

//     // GPIO 초기화
//     XGpio_Initialize(&led_gpio, GPIO_BASEADDR);

//     // 방향 설정(0 = output)
//     XGpio_SetDataDirection(&led_gpio, LED_CHANNEL, 0x0000);

//     while (1) {
//         // 16개 LED 모두 켜기(0xffff = 0b 1111 1111 1111 1111)
//         XGpio_DiscreteWrite(&led_gpio, LED_CHANNEL, 0xffff);
//         sleep(1);

//         // 16개 LED 모두 끄기
//         XGpio_DiscreteWrite(&led_gpio, LED_CHANNEL, 0x0000);
//         sleep(1);
//     }

//     cleanup_platform();
//     return 0;
// }