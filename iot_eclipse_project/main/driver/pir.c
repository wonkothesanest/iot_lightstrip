/*
 * pir.c
 *
 *  Created on: Jul 27, 2018
 *      Author: dustin
 */
#include "pir.h"
#include "mqtt_test.h"
#include "freertos/FreeRTOS.h"
#include "config.h"
#include "gpio_interrupts.h"


void pir_task(void *pvParameter) {
	while(1){
		//Process the Event group and wait a sec before setting clear to the bits
		xEventGroupWaitBits(gpio_evt_grp_pir, BIT0, false, true, portMAX_DELAY);
		vMqttPublish(MQTT_TOPIC_PIR_CMD, "1", sizeof("1"));
		vTaskDelay(10000/portTICK_PERIOD_MS);
		xEventGroupClearBits(gpio_evt_grp_pir, BIT0);
	}
}


void vPirStart() {
	vMqttWaitForConnection();
	xTaskCreate(&pir_task, "PIR Task", 2048, NULL, 5, NULL);
}


