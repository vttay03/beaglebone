/*
 * setupHw.h
 *
 *  Created on: Oct 28, 2012
 *      Author: taylor
 */

#ifndef SETUPHW_H_
#define SETUPHW_H_

#include "beaglebone.h"
#include "hw_types.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_control_AM335x.h"
#include "soc_AM335x.h"
#include "uartStdio.h"
#include "uart_irda_cir.h"

//*****************************************************************************
//
// Macros for hardware access, both direct and via the bit-band region.
//
//*****************************************************************************
#define HWREG(x)                                                              \
        (*((volatile unsigned int *)(x)))

#define TRUE 1
#define FALSE 0

/*****************************************************************************
**                    RESET FUNCTION PROTOTYPES
*****************************************************************************/
void WarmSWReset(void);
void ColdSWReset(void);

/*****************************************************************************
**                    GPIO FUNCTION PROTOTYPES
*****************************************************************************/
void GPIOModuleConfig(int num);
void userLEDConfig(int num);
void setLED(int num, int val);

/*****************************************************************************
**                    UART FUNCTION PROTOTYPES
*****************************************************************************/
// HIGH-LEVEL
void sendXBeeByte(unsigned char data);
unsigned char rcvXBeeByte();
void sendXBeePkt(unsigned char* data, int length);
void packXBeeData(unsigned char* data, int type, int channel, int AVcmd, int pktCnt);
void XBeeUARTInit();

// LOW-LEVEL
void UART1Sendc(unsigned char data);
unsigned char UART1Getc();
void UART1ModuleConfig();

/*****************************************************************************
**                    WDT FUNCTION PROTOTYPES
*****************************************************************************/
void WatchdogTimerSetUp(void);

/*****************************************************************************
**                    DMTIMER FUNCTION PROTOTYPES
*****************************************************************************/
extern volatile unsigned int tmr4Flag;

void DMTimerConfigure();
extern void DMTimerIsr();

void Timer2Start(void);
void Timer2Stop(void);

void Timer4Start(void);
void Timer4Stop(void);

#endif /* SETUPHW_H_ */
