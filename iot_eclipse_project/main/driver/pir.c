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
#include "esp_log.h"

static const char * TAG = "PIR";

void pir_task(void *pvParameter) {
	EventBits_t gpioBits;
	while(1){
		//Process the Event group and wait a sec before setting clear to the bits
		gpioBits = xEventGroupWaitBits(gpio_evt_grp_pir, BIT0, false, true, portMAX_DELAY);
		if((gpioBits & BIT0) != 0){
			vMqttPublish(MQTT_TOPIC_PIR_CMD, "1", sizeof("1"));
			vTaskDelay(PIR_WAIT_BETWEEN_SIGNALS_MS/portTICK_PERIOD_MS);
			xEventGroupClearBits(gpio_evt_grp_pir, BIT0);
		}
	}
}


void vPirStart() {
	ESP_LOGI(TAG, "Starting");

	vMqttWaitForConnection();
	xTaskCreate(&pir_task, "PIR Task", 2048, NULL, 5, NULL);
	ESP_LOGI(TAG, "Finished");

}


