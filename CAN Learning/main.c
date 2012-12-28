/*
 * main.c
 * This is a learning experience to see if I can get the can ports talking
 * to each other on the eval board.
 * Greg Oberfield, 2012
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_i2c.h"
#include "inc/hw_can.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/can.h"

#include "inc/lm4f232h5qd.h"
#include "utils/uartstdio.h" // so I can output to the UART

// Debug handler if driverlib pukes
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{

}
#endif

// initialize the console (from StellarisWare)
void InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioInit(0);
}

int main(void) {
	
	volatile unsigned long returnData[2]; // i know i only get 2 bytes back

	// Set up the system clock, 16Mhz
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// use the uart for console so I can more easily see what's going on
	InitConsole();

	// Set up the user LED on the LM4F232 eval board
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

	CANInit(CAN0_BASE);
	CANInit(CAN1_BASE);
	CANEnable(CAN0_BASE);
	CANEnable(CAN1_BASE);

	while(1) {}
}
