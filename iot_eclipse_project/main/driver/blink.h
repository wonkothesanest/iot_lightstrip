/*
 * blink.h
 *
 *  Created on: Jul 23, 2018
 *      Author: dustin
 */

#ifndef MAIN_DRIVERS_BLINK_H_
#define MAIN_DRIVERS_BLINK_H_

#define BLINK_GPIO 2

void blink_task(void *pvParameter);
void blink_start();


#endif /* MAIN_DRIVERS_BLINK_H_ */
