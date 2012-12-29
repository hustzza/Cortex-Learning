/*
 *  ======== main.c ========
 */

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>

/*
 *  ======== taskFxn ========
 */
void taskFxn(UArg a0, UArg a1)
{
    System_printf("enter taskFxn()\n");

    Task_sleep(10);

    System_printf("exit taskFxn()\n");
}

/*
 *  ======== main ========
 */

void ledToggle()
{
	// Read current state of GPIO pin and
	// write the opposite back

	if(GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_2))
	{
		GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0x00);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0x04);
	}
	SysCtlDelay(1000000);
}

void main()
{ 
    Task_Handle task;
    Error_Block eb;

    System_printf("enter main()\n");

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);
	GPIOPinTypeGPIOInput(GPIO_PORTG_BASE, GPIO_PIN_3);

    Error_init(&eb);
    task = Task_create(taskFxn, NULL, &eb);
    if (task == NULL) {
        System_printf("Task_create() failed!\n");
        BIOS_exit(0);
    }

    BIOS_start();     /* enable interrupts and start SYS/BIOS */
}
