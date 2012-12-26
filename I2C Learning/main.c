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

#include "inc/lm4f232h5qd.h"

#define SLAVE_ADDRESS		0x50

// Debug handler if driverlib pukes
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{

}
#endif

// if we need to send more than a single byte, use this
void i2c_burstsend(unsigned char ucSlaveAddr, unsigned char sendData[], unsigned char size)
{
	unsigned int i;

	// set the address
	I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, ucSlaveAddr, false);
	// send the first byte
	I2CMasterDataPut(I2C0_MASTER_BASE, sendData[0]);
	I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(I2C0_MASTER_BASE))
	{

	}

	// send the middle
	for(i=1; i < (size - 1); i++) {
		I2CMasterDataPut(I2C0_MASTER_BASE, sendData[i]);
		I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
		while(I2CMasterBusy(I2C0_MASTER_BASE))
		{

		}
	}

	// send the last byte
	I2CMasterDataPut(I2C0_MASTER_BASE, sendData[i]);
	I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
	while(I2CMasterBusy(I2C0_MASTER_BASE))
	{

	}
}

int main(void) {
	
	unsigned long ulDataRx[2]; // i know I'm only going to get 2 bytes back from the SHT15
	unsigned long ulIndex;

	// Set up the system clock, 16Mhz
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// Set up the user LED on the LM4F232 eval board
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);

	// Initialize I2C & reset to known state
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	// Set up the pins as I2C
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

	I2CMasterInitExpClk(GPIO_PORTB_BASE, SysCtlClockGet(), false);

	while(1)
	{
		// set the sensor address
		I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, SLAVE_ADDRESS, false);

		// make sure the bus isn't busy
		while(I2CMasterBusBusy(I2C0_MASTER_BASE))
		{

		}

		// transmit command
		I2CMasterDataPut(I2C0_MASTER_BASE, 0x00);
		I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);
		// if master is busy then wait
		while(I2CMasterBusy(I2C0_MASTER_BASE))
		{

		}

		// set the sensor address with command
		I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, SLAVE_ADDRESS, true);

		for(ulIndex = 0; ulIndex < 2; ulIndex++)
		{
			// Set up to receive first byte
			I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

			// Grab the data from the master
			ulDataRx[ulIndex] = I2CMasterDataGet(I2C0_MASTER_BASE);
		}
		SysCtlDelay(100000);  // short delay
	}
}
