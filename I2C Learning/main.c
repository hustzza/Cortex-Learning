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

// VARIABLES
volatile unsigned long returnData = 0;

// if we need to send more than a single byte, use this
void i2c_burstsend(unsigned char ucSlaveAddr, unsigned char sendData[], unsigned char size)
{
	unsigned int i;

	// set the address
	I2CMasterSlaveAddrSet(I2C2_MASTER_BASE, ucSlaveAddr, false);
	// send the first byte
	I2CMasterDataPut(I2C2_MASTER_BASE, sendData[0]);
	I2CMasterControl(I2C2_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(I2C2_MASTER_BASE))
	{

	}

	// send the middle
	for(i=1; i < (size - 1); i++) {
		I2CMasterDataPut(I2C2_MASTER_BASE, sendData[i]);
		I2CMasterControl(I2C2_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
		while(I2CMasterBusy(I2C2_MASTER_BASE))
		{

		}
	}

	// send the last byte
	I2CMasterDataPut(I2C2_MASTER_BASE, sendData[i]);
	I2CMasterControl(I2C2_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
	while(I2CMasterBusy(I2C2_MASTER_BASE))
	{

	}
}

int main(void) {

	// Set up the system clock, 16Mhz
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// Set up the user LED on the LM4F232 eval board
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);

	// Initialize I2C & reset to known state
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	//SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);

	// Set up the pins as I2C
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
	//GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
	//GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);
	//GPIOPinConfigure(GPIO_PA6_I2C1SCL);
	//GPIOPinConfigure(GPIO_PA7_I2C1SDA);

	I2CMasterInitExpClk(GPIO_PORTB_BASE, SysCtlClockGet(), false);
	//I2CMasterInitExpClk(GPIO_PORTA_BASE, SysCtlClockGet(), false);

	I2CMasterEnable(I2C0_MASTER_BASE);

	// set the flash i2c address
	I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, SLAVE_ADDRESS, false); // set to false since it's a write
	I2CMasterDataPut(I2C0_MASTER_BASE, 0x00);	// MSB of address on flash

	// make sure the bus isn't busy
	while(I2CMasterBusBusy(I2C0_MASTER_BASE)) {}

	// start transmit
	I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(I2C0_MASTER_BASE)) {} // delay until complete

	I2CMasterDataPut(I2C0_MASTER_BASE, 0x00); // LSB of address on flash
	I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT); // keep transmitting
	while(I2CMasterBusy(I2C0_MASTER_BASE)) {} // delay until complete

	I2CMasterDataPut(I2C0_MASTER_BASE, 0x2E); // test byte to be written
	I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
	while(I2CMasterBusy(I2C0_MASTER_BASE)) {} // delay until complete

	SysCtlDelay(500000); // short delay to let the EEPROM complete the write

	// set the flash i2c address
	I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, SLAVE_ADDRESS, false); // set to false since it's a write
	I2CMasterDataPut(I2C0_MASTER_BASE, 0x00);	// MSB of address on flash

	// make sure the bus isn't busy
	while(I2CMasterBusBusy(I2C0_MASTER_BASE)) {}

	// start transmit
	I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(I2C0_MASTER_BASE)) {} // delay until complete

	I2CMasterDataPut(I2C0_MASTER_BASE, 0x00); // LSB of address on flash
	I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
	while(I2CMasterBusy(I2C0_MASTER_BASE)) {} // delay until complete

	// now try to read it back
	// set the flash i2c address
	I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, SLAVE_ADDRESS, true); // set to true since it's a read
	while(I2CMasterBusy(I2C0_MASTER_BASE)) {} // delay until complete
	I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
	returnData = I2CMasterDataGet(I2C0_MASTER_BASE);

	// make sure the bus isn't busy
	while(I2CMasterBusBusy(I2C0_MASTER_BASE)) {}

	if(returnData==0x2E)   // quick validation check
	{
		GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0x04);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0x00);
	}

	while(1)
	{
// loop into infinity
	}

}
