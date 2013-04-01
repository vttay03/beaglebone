/*
 * m3pi.h
 *
 *  Created on: Nov 18, 2012
 *      Author: taylor
 */

#ifndef ARDUINO_H_
#define ARDUINO_H_

#include "stdbool.h"

// COMMAND SEQUENCE
//
//
// 10-BYTE PACKET
//
// OFFSET          VALUE          DESCRIPTION
// 0               0x1A
// 1               0xCF
// 2               0xFC
// 3               0x1D
// 4               PKT TYPE       0xA1 = Verizon FIOS, 0xA2 = Yamaha A/V Receiver
// 5               DATA BYTE      Hundreds Channel if PKT TYPE = 0xA1, Command Index if PKT TYPE = 0xA2
// 6               DATA BYTE	  Tens Channel if PKT TYPE = 0xA1, 0xAA if PKT TYPE = 0xA2
// 7               DATA BYTE      Ones Channel if PKT TYPE = 0xA1, 0x55 if PKT TYPE = 0xA2
// 8               DATA BYTE      Command Count (i.e. number of commands transmitted, rollover occurs at 255)
// 9               CHECKSUM       XOR of byte offsets 4 - 8
//
// COMMAND INDEX FOR YAMAHA A/V RECEIVER
// 0x1 = POWER OFF
// 0x2 = POWER ON
// 0x3 = VOLUME DOWN
// 0x4 = VOLUME UP
// 0x5 = CHANGE INPUT DVD
// 0x6 = CHANGE INPUT CABLE
// 0x7 = CHANGE INPUT CD
//

/*****************************************************************************
**                arduino STRUCTURE
*****************************************************************************/
typedef struct {
	int channel; // TV Channel
	int cmdType; // 1 = FIOS TV, 2 = Yamaha A/V Receiver
	int AVcmd; // command number for Yamaha A/V Receiver
	int pktCnt; // Number of packets sent (rollover occurs at 255)
	bool cmdReq; // tells 'main.c' cmd is ready to be transmitted
	unsigned char cmdBuff[10];
	unsigned char tlmBuff[10];
} riu;

extern riu arduino;

#endif /* ARDUINO_H_ */
