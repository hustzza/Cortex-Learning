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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // UART0 is for main debugging

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

//*****************************************************************************
//
// This function is the interrupt handler for the CAN peripheral.  It checks
// for the cause of the interrupt, and maintains a count of all messages that
// have been transmitted.
//
//*****************************************************************************
void
CANIntHandler(void)
{
    unsigned long ulStatus;

    //
    // Read the CAN interrupt status to find the cause of the interrupt
    //
    ulStatus = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    //
    // If the cause is a controller status interrupt, then get the status
    //
    if(ulStatus == CAN_INT_INTID_STATUS)
    {
        //
        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors.  Error processing
        // is not done in this example for simplicity.  Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.  If the
        // CAN peripheral is not connected to a CAN bus with other CAN devices
        // present, then errors will occur and will be indicated in the
        // controller status.
        //
        ulStatus = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);

        //
        // Set a flag to indicate some errors may have occurred.
        //
        g_bErrFlag = 1;
    }

    //
    // Check if the cause is message object 1, which what we are using for
    // sending messages.
    //
    else if(ulStatus == 1)
    {
        //
        // Getting to this point means that the TX interrupt occurred on
        // message object 1, and the message TX is complete.  Clear the
        // message object interrupt.
        //
        CANIntClear(CAN0_BASE, 1);

        //
        // Increment a counter to keep track of how many messages have been
        // sent.  In a real application this could be used to set flags to
        // indicate when a message is sent.
        //
        g_ulMsgCount++;

        //
        // Since the message was sent, clear any error flags.
        //
        g_bErrFlag = 0;
    }

    //
    // Otherwise, something unexpected caused the interrupt.  This should
    // never happen.
    //
    else
    {
        //
        // Spurious interrupt handling can go here.
        //
    }
}

int main(void) {
	
	// Set up the system clock, 16Mhz
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// use the uart for console so I can more easily see what's going on
	InitConsole();

	// Set up the user LED on the LM4F232 eval board
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // enable the peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

	GPIOPinConfigure(GPIO_PE6_CAN1RX); // select the alternate function for the pin
	GPIOPinConfigure(GPIO_PE7_CAN1TX);
	GPIOPinConfigure(GPIO_PN0_CAN0RX);
	GPIOPinConfigure(GPIO_PN1_CAN0TX);

	GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_6 | GPIO_PIN_7); // since alternate function has been selected, now need
	GPIOPinTypeCAN(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1); // to enable alternate functions on the pins

	SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0); // now enable the CAN peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN1);

	CANInit(CAN0_BASE); // initialize the controller
	CANInit(CAN1_BASE);

	CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 125000); // openlcb uses a 125kbit transfer
	CANBitRateSet(CAN1_BASE, SysCtlClockGet(), 125000);

	CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS); // enable interrupts
	CANIntEnable(CAN1_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

	IntEnable(INT_CAN0); // mast interrupt enable
	IntEnable(INT_CAN1);

	CANEnable(CAN0_BASE);
	CANEnable(CAN1_BASE);

	while(1) {}
}
