/*
 * setupWeb.c
 *
 *  Created on: Oct 31, 2012
 *      Author: taylor
 */

#include "delay.h"
#include "boneHw.h"
#include "Web.h"
#include "interrupt.h"
#include "lwiplib.h"
#include "../webserver_src/httpd_io/httpd.h"
#include "locator.h"
#include "lwipopts.h"
#include "arduino.h"
#include "cpsw.h"
#include "uartStdio.h"
#include "cgifuncs.h"

#include "stdbool.h"
#include <stdio.h>
#include <string.h>
/******************************************************************************
**                      INTERNAL MACRO DEFINITIONS
*******************************************************************************/
#define IPADDR_STATIC // needed because compiler wasn't using my local copy of "lwipopts.h"
#define LEN_IP_ADDR                          (4u)
#define ASCII_NUM_IDX                        (48u)
#define CPSW_STAT_TX_COL                     (0x48)
#define CPSW_STAT_RX_CRC_ERR                 (0x10)
#define CPSW_STAT_RX_ALIGN_CODE              (0x14)

#define JAVASCRIPT_HEADER                                                     \
    "<script type='text/javascript' language='JavaScript'><!--\n"
#define JAVASCRIPT_FOOTER                                                     \
    "//--></script>\n"

/******************************************************************************
**                     INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/
void EnetHttpServerInit(void);
void EnetIntRegister(void);
static void IpAddrDisplay(unsigned int ipAddr);
static void CPSWCore0RxIsr(void);
static void CPSWCore0TxIsr(void);
static int SSIHandler(int iIndex, char *pcInsert, int iInsertLen);
static const char* ControlCGIHandler(int iIndex, int iNumParams, char *pcParam[],
                                     char *pcValue[]);
static const char* AVControlCGIHandler(int iIndex, int iNumParams, char *pcParam[],
                                     char *pcValue[]);
/*******************************************************************************
**                     INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/

unsigned int ipAddr;
int LEDState = 0;
int accessCnt = 1; // initialize to '1' so first access shows '1' instead of '0'
LWIP_IF lwipIfPort1, lwipIfPort2;

//*****************************************************************************
//
//! This array is passed to the HTTPD server to inform it of special URIs
//! that are treated as common gateway interface (CGI) scripts.  Each URI name
//! is defined along with a pointer to the function which is to be called to
//! process it.
//
//*****************************************************************************
static const tCGI g_psConfigCGIURIs[] =
{
    { "/tvcontrol.cgi", ControlCGIHandler },
    { "/avcontrol.cgi", AVControlCGIHandler }
};
#define NUM_CONFIG_CGI_URIS     (sizeof(g_psConfigCGIURIs) / sizeof(tCGI))

//*****************************************************************************
//
// This array holds all the strings that are to be recognized as SSI tag
// names by the HTTPD server.  The server will call SSIHandler to request a
// replacement string whenever the pattern <!--#tagname--> (where tagname
// appears in the following array) is found in ".ssi", ".shtml" or ".shtm"
// files that it serves.
//
//*****************************************************************************
#define SSI_INDEX_LEDSTATE  0
#define SSI_INDEX_CHANNEL	1
#define SSI_INDEX_COUNT		2
#define SSI_INDEX_FORMVARS  3

static const char *g_pcConfigSSITags[] =
{
    "LEDtxt",        // SSI_INDEX_LEDSTATE
    "CH",			 // SSI_INDEX_CHANNEL
    "CNT",			 // SSI_INDEX_COUNT
    "FormVars"       // SSI_INDEX_FORMVARS
};
#define NUM_CONFIG_SSI_TAGS     (sizeof(g_pcConfigSSITags) / sizeof (char *))

/*******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/

void WebserverInit()
{

	/* Ethernet PHY port setup */
	CPSWPinMuxSetup();
    CPSWClkEnable();

    /* Chip configuration RGMII selection */
    EVMPortMIIModeSelect();

    /* Get the MAC address */
    EVMMACAddrGet(0, lwipIfPort1.macArray);
    EVMMACAddrGet(1, lwipIfPort2.macArray);

    // No need to initialize the ARM interrupt controller with 'IntAINTCInit()' because
    // it was already done for the Timer.  Otherwise, we'd have to make that function call.

    EnetIntRegister();

    IntPrioritySet(SYS_INT_3PGSWTXINT0, 0, AINTC_HOSTINT_ROUTE_IRQ);
    IntPrioritySet(SYS_INT_3PGSWRXINT0, 0, AINTC_HOSTINT_ROUTE_IRQ);
    IntSystemEnable(SYS_INT_3PGSWTXINT0);
    IntSystemEnable(SYS_INT_3PGSWRXINT0);

    EnetHttpServerInit();
}

/*
** initializes the httpserver and prints the IP address on the UART console
*/
void EnetHttpServerInit(void)
{

    DelayTimerSetup();

    UARTPuts("StarterWare Ethernet Application. Access the embedded"
             " web page using http://<ip address assigned>/io_cgi.ssi"
             " via a web browser. \n\r\n\r" , -1);

    UARTPuts("Acquiring IP Address for Port 1... \n\r" , -1);

#ifdef IPADDR_STATIC

    lwipIfPort1.instNum = 0;
    lwipIfPort1.slvPortNum = 1;
    lwipIfPort1.ipAddr = 0xC0A80128; // 192.168.1.40
    lwipIfPort1.netMask = 0xFFFFFF00; // 255.255.255.0
    lwipIfPort1.gwAddr = 0xC0A80101; // 192.168.1.1
    lwipIfPort1.ipMode = IPADDR_USE_STATIC;

    ipAddr = lwIPInit(&lwipIfPort1);

#else

    lwipIfPort1.instNum = 0;
    lwipIfPort1.slvPortNum = 1;
    lwipIfPort1.ipAddr = 0;
    lwipIfPort1.netMask = 0;
    lwipIfPort1.gwAddr = 0;
    lwipIfPort1.ipMode = IPADDR_USE_DHCP;

    ipAddr = lwIPInit(&lwipIfPort1);

#endif
    if(ipAddr)
    {
        UARTPuts("\n\r\n\rPort 1 IP Address Assigned: ", -1);
        IpAddrDisplay(ipAddr);
    }
    else
    {
        UARTPuts("\n\r\n\rPort 1 IP Address Acquisition Failed.", -1);
    }

    http_set_ssi_handler((tSSIHandler)SSIHandler, g_pcConfigSSITags, NUM_CONFIG_SSI_TAGS);
    http_set_cgi_handlers(g_psConfigCGIURIs, NUM_CONFIG_CGI_URIS);

    /* Initialize the sample httpd server. */
    httpd_init();
}
/*
** Registers ethernet ISRs
*/
void EnetIntRegister(void)
{
    /* Register the Receive ISR for Core 0 */
    IntRegister(SYS_INT_3PGSWRXINT0, CPSWCore0RxIsr);

    /* Register the Transmit ISR for Core 0 */
    IntRegister(SYS_INT_3PGSWTXINT0, CPSWCore0TxIsr);

}

unsigned int EnetIfIsUp(void)
{
   return (lwIPNetIfStatusGet(0, 1));
}

unsigned int EnetLinkIsUp(void)
{

   return (lwIPLinkStatusGet(0, 1));
}

/*
** Interrupt Handler for Core 0 Receive interrupt
*/
static void CPSWCore0RxIsr(void)
{
    lwIPRxIntHandler(0);
}

/*
** Interrupt Handler for Core 0 Transmit interrupt
*/
static void CPSWCore0TxIsr(void)
{
    lwIPTxIntHandler(0);
}

/*
** Displays the IP addrss on the UART Console
*/
static void IpAddrDisplay(unsigned int ipAddr)
{
    unsigned char byte;
    int cnt;

    for(cnt = 0; cnt <= LEN_IP_ADDR - 1; cnt++)
    {
        byte = (ipAddr >> (cnt * 8)) & 0xFF;

        if(cnt)
        {
            UARTPuts(".", -1);
        }

        UARTPutNum((int)byte);
    }
}

/*
** SSI handler
*/
//*****************************************************************************
//
// This function is called by the HTTP server whenever it encounters an SSI
// tag in a web page.  The iIndex parameter provides the index of the tag in
// the g_pcConfigSSITags array. This function writes the substitution text
// into the pcInsert array, writing no more than iInsertLen characters.
//
//*****************************************************************************
static int SSIHandler(int iIndex, char *pcInsert, int iInsertLen)
{

    //
    // Which SSI tag have we been passed?
    //
    switch(iIndex)
    {
		case SSI_INDEX_LEDSTATE: // this writes the value "ON" or "OFF" for each peripheral
			// TODO: Update to actual do a "GPIO_PIN_READ" and make a separate function
			if ( LEDState == 0)
				snprintf(pcInsert, iInsertLen, "OFF");
			else
				snprintf(pcInsert, iInsertLen, "ON");
			break;

		case SSI_INDEX_CHANNEL:
			snprintf(pcInsert, iInsertLen, "%d", arduino.channel);
			break;

		case SSI_INDEX_COUNT:
			snprintf(pcInsert, iInsertLen, "%d", accessCnt);
			break;

        case SSI_INDEX_FORMVARS: // this sets the appropriate values for checkboxes, etc.
            snprintf(pcInsert, iInsertLen,
                      "%sls=%d;\nch=%d;avcmdidx=%d;\n%s",
                      JAVASCRIPT_HEADER,
                      LEDState,
                      arduino.channel,
                      arduino.AVcmd,
                      JAVASCRIPT_FOOTER);
            break;

        default:
			snprintf(pcInsert, iInsertLen, "??");
			break;
    }
    //
    // Tell the server how many characters our insert string contains.
    //
    return(strlen(pcInsert));
}

/*
** 'tvcontrol' CGI handler
*/
//*****************************************************************************
//
// This CGI handler is called whenever the web browser requests tvcontrol.cgi.
//
//*****************************************************************************
static const char* ControlCGIHandler(int iIndex, int iNumParams, char *pcParam[],
                                     char *pcValue[])
{
	bool bParamError = false;
	int checkLED;

	// Variable used below needs to be an extern so that the main timer interrupt routine can
	// access it and update the LED value
    checkLED = FindCGIParameter("LEDOn", pcParam, iNumParams); // use "Find" for checkboxes
    arduino.channel = GetCGIParam("Channel", pcParam, pcValue, iNumParams, &bParamError);
    accessCnt++; // increase access count for display at bottom of page

    // Update LED State
    if (checkLED == -1)
    {
    	LEDState = 0;
    	setLED(3, 0);
    }
    else
    {
    	LEDState = 1;
    	setLED(3, 1);
    }

    // TODO: update this routine to just signal the timer routine
    arduino.cmdType = 1; // 1 = FIOS TV, 2 = Yamaha A/V Receiver
    arduino.cmdReq = true;

    return "/io_cgi.ssi"; // the return value must be the page in which the request came from
}

/*
** 'avcontrol' CGI handler
*/
//*****************************************************************************
//
// This CGI handler is called whenever the web browser requests avcontrol.cgi.
//
//*****************************************************************************
static const char* AVControlCGIHandler(int iIndex, int iNumParams, char *pcParam[],
                                     char *pcValue[])
{
	bool bParamError = false;
    accessCnt++; // increase access count for display at bottom of page

    arduino.AVcmd = GetCGIParam("CMDVal", pcParam, pcValue, iNumParams, &bParamError);
    arduino.cmdType = 2; // 1 = FIOS TV, 2 = Yamaha A/V Receiver
    arduino.cmdReq = true;

    return "/io_cgi.ssi"; // the return value must be the page in which the request came from
}

/****************************** End of file **********************************/
