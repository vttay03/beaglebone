/*
 * hw_setup.c
 *
 *  Created on: May 17, 2012
 *      Author: taylor
 */

#include "boneHw.h"
#include "gpio_v2.h"

void GPIOModuleConfig(int num)
{

	switch(num)
	{
		case 0:
			break;

		case 1:
		    /* Writing to MODULEMODE field of CM_PER_GPIO1_CLKCTRL register. */
		    HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) |=
		          CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE;

		    /* Waiting for MODULEMODE field to reflect the written value. */
		    while(CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE !=
		          (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) &
		           CM_PER_GPIO1_CLKCTRL_MODULEMODE));
		    /*
		    ** Writing to OPTFCLKEN_GPIO_1_GDBCLK bit in CM_PER_GPIO1_CLKCTRL
		    ** register.
		    */
		    HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) |=
		          CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK;

		    /*
		    ** Waiting for OPTFCLKEN_GPIO_1_GDBCLK bit to reflect the desired
		    ** value.
		    */
		    while(CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK !=
		          (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) &
		           CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK));

		    /*
		    ** Waiting for IDLEST field in CM_PER_GPIO1_CLKCTRL register to attain the
		    ** desired value.
		    */
		    while((CM_PER_GPIO1_CLKCTRL_IDLEST_FUNC <<
		           CM_PER_GPIO1_CLKCTRL_IDLEST_SHIFT) !=
		           (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) &
		            CM_PER_GPIO1_CLKCTRL_IDLEST));

		    /*
		    ** Waiting for CLKACTIVITY_GPIO_1_GDBCLK bit in CM_PER_L4LS_CLKSTCTRL
		    ** register to attain desired value.
		    */
		    while(CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_1_GDBCLK !=
		          (HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
		           CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_1_GDBCLK));

		    /* Enabling the GPIO module. */
		    GPIOModuleEnable(SOC_GPIO_1_REGS);

		    /* Resetting the GPIO module. */
		    GPIOModuleReset(SOC_GPIO_1_REGS);

		    break;

		case 2:
			break;

		case 3:
			break;

		default:
			break;
	}

}

void userLEDConfig(int num)
{

	switch (num)
	{
		case 0:
		    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(5)) = CONTROL_CONF_MUXMODE(7); // setup proper pin mux
		    GPIODirModeSet(SOC_GPIO_1_REGS, 21, GPIO_DIR_OUTPUT); // enable GPIO pin as output
			break;

		case 1:
		    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(6)) = CONTROL_CONF_MUXMODE(7); // setup proper pin mux
		    GPIODirModeSet(SOC_GPIO_1_REGS, 22, GPIO_DIR_OUTPUT); // enable GPIO pin as output
			break;

		case 2:
		    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(7)) = CONTROL_CONF_MUXMODE(7); // setup proper pin mux
		    GPIODirModeSet(SOC_GPIO_1_REGS, 23, GPIO_DIR_OUTPUT); // enable GPIO pin as output
			break;

		case 3:
		    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_A(8)) = CONTROL_CONF_MUXMODE(7); // setup proper pin mux
		    GPIODirModeSet(SOC_GPIO_1_REGS, 24, GPIO_DIR_OUTPUT); // enable GPIO pin as output
			break;

		default:
			break;
	}

}

void setLED(int num, int val)
{

	int gpioPin = num + 21; // user led 0 = pin no. 21, user led 1 = pin no. 22, etc.

	if ( val == 1 )
        /* Turn the LED on. */
        GPIOPinWrite(SOC_GPIO_1_REGS, gpioPin, GPIO_PIN_HIGH);
	else
        /* Turn the LED off. */
        GPIOPinWrite(SOC_GPIO_1_REGS, gpioPin, GPIO_PIN_LOW);

}

