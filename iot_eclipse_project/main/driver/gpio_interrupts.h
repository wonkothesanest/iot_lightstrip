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


#define GPIO_INPUT_IO_PIR     4
#define GPIO_INPUT_IO_RNG_FINDER     5
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_PIR) | (1ULL<<GPIO_INPUT_IO_RNG_FINDER))

void vGPIOInterruptStart();
static xQueueHandle gpio_evt_queue = NULL;
static EventGroupHandle_t gpio_evt_grp_pir = NULL;
static EventGroupHandle_t gpio_evt_grp_rng_finder = NULL;





#endif /* MAIN_DRIVER_GPIO_INTERRUPTS_H_ */
