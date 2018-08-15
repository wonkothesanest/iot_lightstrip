/*
 * gpio_interrupts.c
 *
 *  Created on: Jul 26, 2018
 *      Author: dustin
 */
#include "gpio_interrupts.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "config.h"
#include "freertos/event_groups.h"
#include "mqtt_test.h"


#define ESP_INTR_FLAG_DEFAULT 0


static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}


static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
        	if(io_num == GPIO_INPUT_IO_PIR){
        		xEventGroupSetBits(gpio_evt_grp_pir, BIT0);
        	}else if(io_num == GPIO_INPUT_IO_RNG_FINDER){
        		//xEventGroupSetBits(gpio_evt_grp_rng_finder, BIT0);
        	}
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}


void vGPIOInterruptStart() {
    gpio_config_t io_conf;
    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    //change gpio intrrupt type for two pins
    gpio_set_intr_type(GPIO_INPUT_IO_PIR, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(GPIO_INPUT_IO_RNG_FINDER, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    gpio_evt_grp_pir = xEventGroupCreate();

    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_PIR, gpio_isr_handler, (void*) GPIO_INPUT_IO_PIR);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_RNG_FINDER, gpio_isr_handler, (void*) GPIO_INPUT_IO_RNG_FINDER);


}

