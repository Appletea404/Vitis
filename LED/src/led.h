#include "def.h"
#include <xgpio.h>
#include <xil_types.h>

#define GPIO_BASEADDR   XPAR_AXI_GPIO_0_BASEADDR 
#define LED_CHANNEL     1                           // GPIO 채널 1 사용 

int LED_Init(XGpio *InstancePtr);
void LED_ALLOn(XGpio *InstancePtr);
void LED_ALLOff(XGpio *InstancePtr);
void LED_MoveLeft(XGpio *InstancePtr, u32 delay_us);
void LED_MoveRight(XGpio *InstancePtr, u32 delay_us);
void LED_KnightRider(XGpio *InstancePtr, u32 delay_us);
void LED_CenterOut(XGpio *InstancePtr, u32 delay_us);