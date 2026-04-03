#include "led.h"
#include <sleep.h>
#include <stdint.h>
#include <xgpio.h>
#include <xil_types.h>
#include <xstatus.h>




int LED_Init(XGpio *InstancePtr)
{
    if (XGpio_Initialize(InstancePtr, GPIO_BASEADDR) != XST_SUCCESS)
    {
        return XST_FAILURE;
    }
    XGpio_SetDataDirection(InstancePtr, LED_CHANNEL, 0x0000);
    return XST_SUCCESS;
}
void LED_ALLOn(XGpio *InstancePtr)
{
    XGpio_DiscreteWrite(InstancePtr, LED_CHANNEL, 0xFFFF);
}
void LED_ALLOff(XGpio *InstancePtr)
{
    XGpio_DiscreteWrite(InstancePtr, LED_CHANNEL, 0x0000);
}
void LED_MoveLeft(XGpio *InstancePtr, u32 delay_us)
{
    static u16 led;
    XGpio_DiscreteWrite(InstancePtr, LED_CHANNEL, led);
    led <<= 1;
    usleep(delay_us);
    if (led == 0)
    {
        led = 0x0001;
    }
}
void LED_MoveRight(XGpio *InstancePtr, u32 delay_us)
{
    static u16 led;
    XGpio_DiscreteWrite(InstancePtr, LED_CHANNEL, led);
    led >>= 1;
    usleep(delay_us);
    if (led == 0)
    {
        led = 0x8000;
    }
}
void LED_KnightRider(XGpio *InstancePtr, u32 delay_us)
{
    static u16 led = 0x0001;
    for(int i = 0; i < 16; i++)
    {
        XGpio_DiscreteWrite(InstancePtr, LED_CHANNEL, led);
        led <<= 1;
        usleep(delay_us);    }

        if (led == 0)
    {
        led = 0x8000;
    }
    for(int j = 0; j < 16; j++)
    {
      XGpio_DiscreteWrite(InstancePtr, LED_CHANNEL, led);
      led >>= 1;
         usleep(delay_us);
    }
    if (led == 0)
    {
        led = 0x0001;
    }

    
    

    
}
void LED_CenterOut(XGpio *InstancePtr, u32 delay_us)
{
    u16 left_half = 0x0100;
    u16 right_half = 0x0080;
    for (int i = 0; i < 8; i++) {
    XGpio_DiscreteWrite(InstancePtr,LED_CHANNEL , left_half|right_half);
    usleep(delay_us);
    left_half <<= 1;
    right_half >>= 1;

    }
}