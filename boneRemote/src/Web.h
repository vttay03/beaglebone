/*
 * setupWeb.h
 *
 *  Created on: Oct 31, 2012
 *      Author: taylor
 */

#ifndef SETUPWEB_H_
#define SETUPWEB_H_

/*
** The indexes of Each Icon in the html page
*/
#define CLICK_IDX_RTC                          (6u)
#define CLICK_IDX_LED                          (9u)
#define CLICK_IDX_TIMER                        (7u)
#define CLICK_IDX_SD			               (2u)

/******************************************************************************
**                      EXTERNAL VARIABLE DECLARATIONS
*******************************************************************************/
extern unsigned int clickIdx;
extern unsigned int enetInitFlag;

void WebserverInit();

extern unsigned int EnetIfIsUp(void);
extern unsigned int EnetLinkIsUp(void);

#endif /* SETUPWEB_H_ */
