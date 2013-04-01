/*
 * setupUart.c
 *
 *  Created on: Oct 25, 2012
 *      Author: taylor
 */

#include "boneHw.h"
#include "uart_irda_cir.h"

void sendXBeeByte(unsigned char data) {
	UART1Sendc(data);
}

unsigned char rcvXBeeByte() {
	return(UART1Getc());
}

void sendXBeePkt(unsigned char* data, int length)
{
	int i;

	for(i = 0;i < length; i++)
		sendXBeeByte(data[i]);

}

void packXBeeData(unsigned char* data, int type, int channel, int AVcmd, int pktCnt)
{
	int i;
	int hundreds, tens, ones;

	// Assign sync
	data[0] = 0x1A;
	data[1] = 0xCF;
	data[2] = 0xFC;
	data[3] = 0x1D;

	// Assign packet type (0xA1 = Verizon FIOS, 0xA2 = Yamaha A/V Receiver)
	switch(type)
	{
		case 1:
			hundreds = channel/100;
			tens = (channel - (hundreds*100))/10;
			ones = (channel - (hundreds*100) - (tens*10));

			data[4] = 0xA1;
			data[5] = (unsigned char)hundreds;
			data[6] = (unsigned char)tens;
			data[7] = (unsigned char)ones;
			break;

		case 2:
			data[4] = 0xA2;
			data[5] = (unsigned char)AVcmd;
			data[6] = 0xAA;
			data[7] = 0x55;
			break;

		default: // Error condition?
			data[4] = 0xFF;
			data[5] = 0x00;
			data[6] = 0x00;
			data[7] = 0x00;
			break;
	}

	data[8] = (unsigned char)pktCnt;

	// Compute checksum (XOR bytes excluding sync)
	data[9] = 0x00; // initialize checksum
	for (i = 4;i < 9;i++)
	{
		data[9] ^= data[i];
	}

}

void XBeeUARTInit() {

    /* XBee Link will use UART1 */

    /* Configuring the system clocks for UART1 instance. */
    UART1ModuleConfig();

}

