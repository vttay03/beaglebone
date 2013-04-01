/*
 * reset.c
 *
 *  Created on: Nov 13, 2012
 *      Author: taylor
 */


#include "boneHw.h"

#define PRM_DEVICE_RSTCTRL   			(0x44E00F00)
#define PRM_DEVICE_RST_GLOBAL_WARM_SW	(0x00000001u)
#define PRM_DEVICE_RST_GLOBAL_COLD_SW	(0x00000002u)

// Triggers a global warm software reset
void WarmSWReset() {

	HWREG(PRM_DEVICE_RSTCTRL) |= PRM_DEVICE_RST_GLOBAL_WARM_SW;

}

void ColdSWReset() {

	HWREG(PRM_DEVICE_RSTCTRL) |= PRM_DEVICE_RST_GLOBAL_COLD_SW;

}
