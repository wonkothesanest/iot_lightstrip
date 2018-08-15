/*
 * gpio_interrupts.h
 *
 *  Created on: Jul 26, 2018
 *      Author: dustin
 */

#ifndef MAIN_DRIVER_GPIO_INTERRUPTS_H_
#define MAIN_DRIVER_GPIO_INTERRUPTS_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "config.h"

/**
 * All pin interrupts happen here, so far this is just the PIR and
 * the Range finder
 *
 * If we decide to do an IR input we should probably break out into
 * multiple libraries and record microsecond time from ISR
 */


#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_PIR) | (1ULL<<GPIO_INPUT_IO_RNG_FINDER))

void vGPIOInterruptStart();
//Queue handle for any GPIO interrupt event
static xQueueHandle gpio_evt_queue;
//Event Group to notify PIR
EventGroupHandle_t gpio_evt_grp_pir;





#endif /* MAIN_DRIVER_GPIO_INTERRUPTS_H_ */
