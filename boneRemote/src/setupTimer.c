/*
 * setupTimer.c
 *
 *  Created on: Oct 28, 2012
 *      Author: taylor
 */

#include "dmtimer.h"
#include "interrupt.h"
#include "boneHw.h"


/******************************************************************************
**                      INTERNAL MACRO DEFINITIONS
*******************************************************************************/
#define TIMER_INITIAL_COUNT             (0xFE91C9FFu)
#define TIMER_RLD_COUNT                 (0xFE91C9FFu)
#define TIMER_FINAL_COUNT               (0x0FFFFu)

/*	1 count at 32.768 KHz takes 31.25us	*/
/*	0x1900 counts takes 200ms */
#define TIMER_200MS_DELAY               (0xFFFFE6FF)

/*******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/
void Timer2IntRegister(void);
void Timer2Config(void);
void Timer2IntEnable(void);
void Timer2Start(void);
void Timer2Stop(void);

void Timer4IntRegister(void);
void Timer4Config(void);
void Timer4IntEnable(void);
void Timer4Start(void);
void Timer4Stop(void);

static void Timer4Isr(void);

/*******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/
volatile unsigned int tmr4Flag = FALSE;

/*******************************************************************************
**                     FUNCTION DEFINITIONS TIMER2
*******************************************************************************/
void DMTimerConfigure() {

    /* This function will enable clocks for the DMTimer2 and DMTimer4 instances */
    DMTimer2ModuleClkConfig();
    //DMTimer4ModuleClkConfig();

    /* Register DMTimer2 interrupts on to AINTC */
//    DMTimerAintcConfigure();

    /* Enable IRQ in CPSR */
//    IntMasterIRQEnable();

    /* Register Timer ISRs */
    Timer2IntRegister();
    //Timer4IntRegister();

    /* Perform the necessary configurations for DMTimer */
    Timer2Config();
    //Timer4Config();

    Timer2IntEnable();
    //Timer4IntEnable();

}

/*
** Do the necessary DMTimer configurations on to AINTC.
*/
void Timer2IntRegister(void)
{
    /* Initialize the ARM interrupt control */
    //IntAINTCInit();

    /* Registering DMTimerIsr */
    IntRegister(SYS_INT_TINT2, DMTimerIsr);

    /* Set the priority */
    IntPrioritySet(SYS_INT_TINT2, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enable the system interrupt */
    IntSystemEnable(SYS_INT_TINT2);
}

/*
** Setup the timer for one-shot and compare mode.
*/
void Timer2Config(void)
{
    /* Load the counter with the initial count value */
    DMTimerCounterSet(SOC_DMTIMER_2_REGS, TIMER_INITIAL_COUNT);

    /* Load the load register with the reload count value */
    DMTimerReloadSet(SOC_DMTIMER_2_REGS, TIMER_RLD_COUNT);

    /* Configure the DMTimer for Auto-reload and compare mode */
    DMTimerModeConfigure(SOC_DMTIMER_2_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);

    Timer2Stop();
}

/*
** Enables the Timer2 Interrupts
*/
void Timer2IntEnable(void)
{
    /* Enable the DMTimer interrupts */
    DMTimerIntEnable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);

}

/*
** Starts the Timer
*/
void Timer2Start(void)
{
    /* Start the DMTimer */
    DMTimerEnable(SOC_DMTIMER_2_REGS);
}

/*
** Stops the Timer. The Timer Counter is Reset.
*/
void Timer2Stop(void)
{
    DMTimerDisable(SOC_DMTIMER_2_REGS);
    //DMTimerCounterSet(SOC_DMTIMER_4_REGS, 0);
}

/*******************************************************************************
**                     FUNCTION DEFINITIONS TIMER4
*******************************************************************************/
/*
** Registers the Timer4 ISR.
*/
void Timer4IntRegister(void)
{
    IntRegister(SYS_INT_TINT4, Timer4Isr);

    /* Set the priority */
    IntPrioritySet(SYS_INT_TINT4, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enable the system interrupt */
    IntSystemEnable(SYS_INT_TINT4);
}

/*
** Configures the Timer4 for 32 bit
*/
void Timer4Config(void)
{
    /* Load the counter with the initial count value */
    DMTimerCounterSet(SOC_DMTIMER_4_REGS, TIMER_INITIAL_COUNT);

    /* Load the load register with the reload count value */
    DMTimerReloadSet(SOC_DMTIMER_4_REGS, TIMER_200MS_DELAY);

    /* Configure the DMTimer for one shot mode */
    DMTimerModeConfigure(SOC_DMTIMER_4_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);

	Timer4Stop();
}

/*
** Enables the Timer4 Interrupts
*/
void Timer4IntEnable(void)
{
    /* Enable the DMTimer interrupts */
    DMTimerIntEnable(SOC_DMTIMER_4_REGS, DMTIMER_INT_OVF_EN_FLAG);

}

/*
** Starts the Timer
*/
void Timer4Start(void)
{
    /* Start the DMTimer */
    DMTimerEnable(SOC_DMTIMER_4_REGS);
}

/*
** Stops the Timer. The Timer Counter is Reset.
*/
void Timer4Stop(void)
{
    DMTimerDisable(SOC_DMTIMER_4_REGS);
    //DMTimerCounterSet(SOC_DMTIMER_4_REGS, 0);
}

/*
** Timer 4 Interrupt Service Routine
*/
static void Timer4Isr(void)
{
    /* Clear the status of the interrupt flags */
    DMTimerIntStatusClear(SOC_DMTIMER_4_REGS, DMTIMER_INT_OVF_EN_FLAG);

    tmr4Flag = TRUE;

	//DMTimerEnable(SOC_DMTIMER_4_REGS);

}

