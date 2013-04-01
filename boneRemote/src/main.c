/*
 * main.c
 */

#include "cache.h"
#include "delay.h"
#include "dmtimer.h"
#include "interrupt.h"
#include "lwiplib.h"
#include "arduino.h"
#include "mmu.h"
#include "boneHw.h"
#include "Web.h"
#include "string.h"
#include "stdbool.h"
#include "stdlib.h"
#include "watchdog.h"

/******************************************************************************
**              INTERNAL MACRO DEFINITIONS
******************************************************************************/
#define UART_CONSOLE_BASE                    (SOC_UART_0_REGS)
#define UART_XBEE_BASE                    	 (SOC_UART_1_REGS)
#define prdebug UARTprintf
#define LEN_IP_ADDR                        (4u)

#define START_ADDR_DDR                     (0x80000000)
#define START_ADDR_DEV                     (0x44000000)
#define START_ADDR_OCMC                    (0x40300000)
#define NUM_SECTIONS_DDR                   (512)
#define NUM_SECTIONS_DEV                   (960)
#define NUM_SECTIONS_OCMC                  (1)

static volatile unsigned int cntValue = 0;
static volatile int heartbeat = 0;
unsigned int triggerValue = 0;

/****************************************************************************
**                   EXTERNAL VARIABLE DECLARATIONS
****************************************************************************/
extern unsigned int ipAddr;
extern volatile tBoolean bConnected;

riu arduino;

/*******************************************************************************
**                     EXTERNAL FUNCTION DECLARATIONS
*******************************************************************************/
extern void etharp_tmr(void);

/****************************************************************************
**                  GLOBAL VARIABLES DEFINITIONS
****************************************************************************/
unsigned int clickIdx = 0;

/******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/
/* page tables start must be aligned in 16K boundary */
#ifdef __TMS470__
#pragma DATA_ALIGN(pageTable, 16384);
static volatile unsigned int pageTable[4*1024];
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=16384
static volatile unsigned int pageTable[4*1024];
#else
static volatile unsigned int pageTable[4*1024] __attribute__((aligned(16*1024)));
#endif

/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/
extern void DMTimerIsr(void);
static void MMUConfigAndEnable(void);
/*****************************************************************************
**                INTERNAL FUNCTION DEFINITIONS
*****************************************************************************/

/*
** Function to setup MMU. This function Maps three regions (1. DDR
** 2. OCMC and 3. Device memory) and enables MMU.
*/
static void MMUConfigAndEnable(void)
{
    /*
    ** Define DDR memory region of AM335x. DDR can be configured as Normal
    ** memory with R/W access in user/privileged modes. The cache attributes
    ** specified here are,
    ** Inner - Write through, No Write Allocate
    ** Outer - Write Back, Write Allocate
    */
    REGION regionDdr = {
                        MMU_PGTYPE_SECTION, START_ADDR_DDR, NUM_SECTIONS_DDR,
                        MMU_MEMTYPE_NORMAL_NON_SHAREABLE(MMU_CACHE_WT_NOWA,
                                                         MMU_CACHE_WB_WA),
                        MMU_REGION_NON_SECURE, MMU_AP_PRV_RW_USR_RW,
                        (unsigned int*)pageTable
                       };
    /*
    ** Define OCMC RAM region of AM335x. Same Attributes of DDR region given.
    */
    REGION regionOcmc = {
                         MMU_PGTYPE_SECTION, START_ADDR_OCMC, NUM_SECTIONS_OCMC,
                         MMU_MEMTYPE_NORMAL_NON_SHAREABLE(MMU_CACHE_WT_NOWA,
                                                          MMU_CACHE_WB_WA),
                         MMU_REGION_NON_SECURE, MMU_AP_PRV_RW_USR_RW,
                         (unsigned int*)pageTable
                        };

    /*
    ** Define Device Memory Region. The region between OCMC and DDR is
    ** configured as device memory, with R/W access in user/privileged modes.
    ** Also, the region is marked 'Execute Never'.
    */
    REGION regionDev = {
                        MMU_PGTYPE_SECTION, START_ADDR_DEV, NUM_SECTIONS_DEV,
                        MMU_MEMTYPE_DEVICE_SHAREABLE,
                        MMU_REGION_NON_SECURE,
                        MMU_AP_PRV_RW_USR_RW  | MMU_SECTION_EXEC_NEVER,
                        (unsigned int*)pageTable
                       };

    /* Initialize the page table and MMU */
    MMUInit((unsigned int*)pageTable);

    /* Map the defined regions */
    MMUMemRegionMap(&regionDdr);
    MMUMemRegionMap(&regionOcmc);
    MMUMemRegionMap(&regionDev);

    /* Now Safe to enable MMU */
    MMUEnable((unsigned int*)pageTable);
}

/*
** The main function. Application starts here.
*/
void main(void) {

    /* Initialize the ARM Interrupt Controller */
    IntAINTCInit();

    MMUConfigAndEnable();
#ifdef LWIP_CACHE_ENABLED
	CacheEnable(CACHE_ALL);
#endif

	// Initialization routines for hardware needed
    UARTStdioInit(); // initialize debug console
	prdebug("Initializing hardware...\n");

	/* Enabling functional clocks for GPIO1 instance. */
    GPIOModuleConfig(1);
    /* Enabling user LEDs for use. */
    userLEDConfig(0);
    userLEDConfig(1);
    userLEDConfig(2);
    userLEDConfig(3);
    XBeeUARTInit(); // operates at 115200 baud
    WatchdogTimerSetUp(); // sets watchdog timer to reset after 16 secs of no activity
	DMTimerConfigure();

    IntMasterIRQEnable();
    IntSystemEnable(SYS_INT_TINT2);
    IntPrioritySet(SYS_INT_TINT2, 0, AINTC_HOSTINT_ROUTE_IRQ);
    //IntSystemEnable(SYS_INT_TINT4);
    //IntPrioritySet(SYS_INT_TINT4, 0, AINTC_HOSTINT_ROUTE_IRQ);

    // Start the timers
    Timer2Start();
    //Timer4Start();

    prdebug("Hardware initialization complete!\n");
	prdebug("Press any key within 10 seconds to enter local mode...\n");

	// initialize arduino remote interface unit (RIU) struct
	arduino.channel = 0;
	arduino.AVcmd = 8;
	arduino.pktCnt = 0;
	arduino.cmdType = 1; // default to Verizon FIOS (Yamaha A/V Receiver not currently implemented)
	arduino.cmdReq = false;

	while(cntValue < 10)
    {
    	if ( UARTCharsAvail(UART_CONSOLE_BASE) == TRUE ) // Doesn't work --> dig into what this function is ACTUALLY returning
		{
				UARTGetc(); // flush buffer
				goto local_mode;
		}
    }

	goto remote_mode;

    local_mode:
		prdebug("Entering local mode...\n");
    	/* Stop the DMTimers */
		//Timer2Stop();
		//Timer4Stop();
    	while(1);

    remote_mode:
    	prdebug("Entering remote mode...\n");
    	WebserverInit();
        while(1)
		{
        	if ( UARTCharsAvail(UART_CONSOLE_BASE) == TRUE )
        		goto cleanup_and_exit;
		};

    cleanup_and_exit:
		/* Stop the DMTimers */
    	Timer2Stop();
    	IntMasterIRQDisable();
    	WarmSWReset();
        //Timer4Stop();
    	while(1); // halt


}

/*
** DMTimer interrupt service routine. This will send a character to serial
** console.
*/
extern void DMTimerIsr(void)
{
	int i;

    /* Disable the DMTimer interrupts */
    DMTimerIntDisable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);

    /* Clear the status of the interrupt flags */
    DMTimerIntStatusClear(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);

    triggerValue += 1;

    /* Write into the WDT trigger register. This will load the value from the
    ** load register into the counter register and hence timer will start
    ** from the initial count.
    */
    WatchdogTimerTriggerSet(SOC_WDT_1_REGS, triggerValue);

    cntValue++;
    prdebug("qt");

    heartbeat ^= 1; // flip LED
    setLED(0, heartbeat);

    if (arduino.cmdReq == true) // send command signal set by CGI Handler in "setupWeb.c"
    {

    	// pack command and transmit
    	prdebug("Send Arduino Command Packet\n");
    	arduino.pktCnt++;
    	if ( arduino.pktCnt == 256 )
    		arduino.pktCnt = 0;
    	packXBeeData(arduino.cmdBuff, arduino.cmdType, arduino.channel, arduino.AVcmd, arduino.pktCnt); // create packet
    	sendXBeePkt(arduino.cmdBuff, 10); // send packet
    	for(i = 0; i < 10; i++)
    	{
    		prdebug("Byte[%d]: %02X\n", i, arduino.cmdBuff[i]);
    	}
    	arduino.cmdReq = false;
    }

	while (UARTCharsAvail(UART_XBEE_BASE) == TRUE ) {
    	prdebug("%c", rcvXBeeByte());
    }

    /* Enable the DMTimer interrupts */
    DMTimerIntEnable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);
}

/******************************* End of file *********************************/

