/*
 * main.c
 * This is a learning experience to figure out I2C use on the Stellaris MCU family
 * Greg Oberfield, 12/26/2012
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_i2c.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"

#include "inc/lm4f232h5qd.h"	// this turned out to be pretty damn important
								// so I would have the pin definitions for the MCU

int main(void) {
	
	// Set up the system clock, 16Mhz
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// Set up the user LED on the LM4F232 eval board
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);

	// Set up the pins as I2C
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);

	// Initialize I2C & reset to known state
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

	I2CMasterInitExpClk(GPIO_PORTB_BASE, SysCtlClockGet(), false);
	//SysCtlDelay(10000); // apparently must delay here or will lose slave address
}
