/*
 * setupUart_l.c
 *
 *  Created on: Oct 27, 2012
 *      Author: taylor
 */
#include "boneHw.h"
#include "uart_irda_cir.h"

/******************************************************************************
**              INTERNAL MACRO DEFINITIONS
******************************************************************************/
#define UART_XBEE_BASE                    (SOC_UART_1_REGS)
#define BAUD_RATE_115200                     (115200)
#define BAUD_RATE_9600						 (9600)
#define UART_MODULE_INPUT_CLK                (48000000)

/******************************************************************************
**              FUNCTION DEFINITIONS
******************************************************************************/

/**
 * \brief   This function sends a character on the XBee link.
 *
 * \param   data    The character to be sent on the XBee link
 *
 * \return  None.
 */
void UART1Sendc(unsigned char data)
{
     UARTCharPut(UART_XBEE_BASE, data);
}

/**
 * \brief   This function receives a character on the XBee link.
 *
 * \param   none
 *
 * \return  Character as input from the XBee
 */
unsigned char UART1Getc(void)
{
     return ((unsigned char)UARTCharGet(UART_XBEE_BASE));
}

/**
 * \brief   This function initializes the specified UART instance for use.
 *          This does the following:
 *          - Enables the FIFO and performs the FIFO settings\n
 *          - Performs the Baud Rate settings\n
 *          - Configures the Line Characteristics to 8-N-1 format\n
 *
 * \param   baudRate     The baud rate to be used for communication
 * \param   rxTrigLevel  The receiver FIFO trigger level
 * \param   txTrigLevel  The transmitter FIFO trigger level
 *
 * \return  None
 *
 * \note    By default, a granularity of 1 will be selected for transmitter
 *          and receiver FIFO trigger levels.
 */

void UART1InitExpClk(unsigned int baudRate,
                            unsigned int rxTrigLevel,
                            unsigned int txTrigLevel)
{
    unsigned int fifoConfig = 0;
    unsigned int divisorValue = 0;

    /* Performing a module reset. */
    UARTModuleReset(UART_XBEE_BASE);

    /* Performing FIFO configurations. */

    /* Setting the TX and RX FIFO Trigger levels as 1. No DMA enabled. */
    fifoConfig = UART_FIFO_CONFIG(UART_TRIG_LVL_GRANULARITY_1,
                                  UART_TRIG_LVL_GRANULARITY_1,
                                  txTrigLevel,
                                  rxTrigLevel,
                                  1,
                                  1,
                                  UART_DMA_EN_PATH_SCR,
                                  UART_DMA_MODE_0_ENABLE);

    /* Configuring the FIFO settings. */
    UARTFIFOConfig(UART_XBEE_BASE, fifoConfig);

    /* Performing Baud Rate settings. */

    /* Computing the Divisor Value. */
    divisorValue = UARTDivisorValCompute(UART_MODULE_INPUT_CLK,
                                         baudRate,
                                         UART16x_OPER_MODE,
                                         UART_MIR_OVERSAMPLING_RATE_42);

    /* Programming the Divisor Latches. */
    UARTDivisorLatchWrite(UART_XBEE_BASE, divisorValue);

    /* Switching to Configuration Mode B. */
    UARTRegConfigModeEnable(UART_XBEE_BASE, UART_REG_CONFIG_MODE_B);

    /* Programming the Line Characteristics. */
    UARTLineCharacConfig(UART_XBEE_BASE,
                         (UART_FRAME_WORD_LENGTH_8 | UART_FRAME_NUM_STB_1),
                         UART_PARITY_NONE);

    /* Disabling write access to Divisor Latches. */
    UARTDivisorLatchDisable(UART_XBEE_BASE);

    /* Disabling Break Control. */
    UARTBreakCtl(UART_XBEE_BASE, UART_BREAK_COND_DISABLE);

    /* Switching to UART16x operating mode. */
    UARTOperatingModeSelect(UART_XBEE_BASE, UART16x_OPER_MODE);
}

void UART1ModuleConfig() {
    /* Configuring registers related to Per region. */

    /* Writing to MODULEMODE field of CM_PER_L3_INSTR_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) |=
    	  CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE;

    /*Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_L3_INSTR_CLKCTRL) &
           CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE));

    /* Writing to MODULEMODE field of CM_PER_L3_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) |=
    	  CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE;

    /*Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKCTRL) &
           CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE));

    /* Writing to MODULEMODE field of CM_PER_L4LS_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) |=
    		CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE;

    /*Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKCTRL) &
           CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE));

    /* Writing to MODULEMODE field of CM_PER_L4FW_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_L4FW_CLKCTRL) |=
    		CM_PER_L4FW_CLKCTRL_MODULEMODE_ENABLE;

    /*Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_L4FW_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_L4FW_CLKCTRL) &
           CM_PER_L4FW_CLKCTRL_MODULEMODE_ENABLE));

    /* Writing to CLKTRCTRL field of CM_PER_L4LS_CLKSTCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) |=
    		CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    /* Waiting for CLKTRCTRL field to reflect the written value. */
    while(CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
           CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP));

    /* Writing to CLKTRCTRL field of CM_PER_L3S_CLKSTCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) |=
    		CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    /*Waiting for CLKTRCTRL field to reflect the written value. */
    while(CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_L3S_CLKSTCTRL) &
           CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP));

    /* Writing to CLKTRCTRL field of CM_PER_L3_CLKSTCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) |=
    		CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    /*Waiting for CLKTRCTRL field to reflect the written value. */
    while(CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_L3_CLKSTCTRL) &
           CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP));

    /* Writing to MODULEMODE field of CM_PER_UART1_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_UART1_CLKCTRL) |=
    		CM_PER_UART1_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_UART1_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_UART1_CLKCTRL) &
           CM_PER_UART1_CLKCTRL_MODULEMODE_ENABLE));

    /* Waiting for IDLEST field in CM_PER_UART1_CLKCTRL register to attain desired value. */
    while((CM_PER_UART1_CLKCTRL_IDLEST_FUNC <<
    		CM_PER_UART1_CLKCTRL_IDLEST_SHIFT) !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_UART1_CLKCTRL) &
        		  CM_PER_UART1_CLKCTRL_IDLEST));

    /* Now setup Pin multiplexing */

    /* RXD */
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_UART_RXD(1)) =
    (CONTROL_CONF_UART1_RXD_CONF_UART1_RXD_PUTYPESEL |
     CONTROL_CONF_UART1_RXD_CONF_UART1_RXD_RXACTIVE);

    /* TXD */
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_UART_TXD(1)) =
     CONTROL_CONF_UART1_TXD_CONF_UART1_TXD_PUTYPESEL;

    UART1InitExpClk(BAUD_RATE_9600, 1, 1);
}
