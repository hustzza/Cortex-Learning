/*
 * main.c
 * Greg Oberfield
 * Uses Stellaris Launchpad (LM4F120H5QR)
 * Testing various interrupts, GPIO and timer
 */

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/fpu.h"

int pin = GPIO_PIN_1; // configure to start with pin 1

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

void GPIOFIntHandler(void)
{
	GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
	GPIOPinWrite(GPIO_PORTF_BASE, pin, 0);

	switch (pin)
	{
	case GPIO_PIN_1:
		pin = GPIO_PIN_2;
		break;
	case GPIO_PIN_2:
		pin = GPIO_PIN_3;
		break;
	case GPIO_PIN_3:
		pin = GPIO_PIN_1;
		break;
	}

}

void Timer0IntHandler(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	if(GPIOPinRead(GPIO_PORTF_BASE, pin))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, pin, 0);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, pin, 14);
	}
}

int main(void) {
	
	FPULazyStackingEnable();

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); // Clock speed 40Mhz (400/2/5)

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	// Extremely annoying but it turns out that we have to alter registers before using the launchpad
	// switch.  Sheesh.
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE+ GPIO_O_LOCK) = 0;

	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); // configure LED pins as outputs
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0); // turn off LEDs
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0); // SW1 set as input
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE); // set interrupt to falling edge

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);

	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()-1);  // set to 1 full second

	IntEnable(INT_TIMER0A);
	IntEnable(INT_GPIOF);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);

	while(1)
	{

	}
}
