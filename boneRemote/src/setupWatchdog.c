/*
 * setupWatchdog.c
 *
 *  Created on: Oct 28, 2012
 *      Author: taylor
 */


#include "watchdog.h"
#include "boneHw.h"

#define INITIAL_COUNT_VALUE          (0xFFF80000u) // reset will occur after ~16 sec with no activity
#define RELOAD_COUNT_VALUE           (0xFFF80000u)

void WatchdogTimerSetUp(void) {

    /* Enable the WDT clocks */
    WatchdogTimer1ModuleClkConfig();

    /* Reset the Watchdog Timer */
    WatchdogTimerReset(SOC_WDT_1_REGS);

    /* Disable the Watchdog timer */
    WatchdogTimerDisable(SOC_WDT_1_REGS);

    /* Configure and enable the pre-scaler clock */
    WatchdogTimerPreScalerClkEnable(SOC_WDT_1_REGS, WDT_PRESCALER_CLK_DIV_1);

    /* Set the count value into the counter register */
    WatchdogTimerCounterSet(SOC_WDT_1_REGS, INITIAL_COUNT_VALUE);

    /* Set the reload value into the load register */
    WatchdogTimerReloadSet(SOC_WDT_1_REGS, RELOAD_COUNT_VALUE);

    /* Enable the Watchdog Timer */
    WatchdogTimerEnable(SOC_WDT_1_REGS);

}
