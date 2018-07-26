/*
 * blink.c
 *
 *  Created on: Jul 23, 2018
 *      Author: dustin
 */
#include "blink.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "sdkconfig.h"
#include "freertos/queue.h"

#include "mqtt_test.h"
#include "mqtt_client.h"
#include <string.h>
#include "esp_log.h"

static const char * TAG = "Blink Example";
const char * blink_pub_topic = "/home/light/state";
const char * blink_sub_topic = "/home/light/command";
QueueHandle_t light_queue;
#define LIGHT_QUEUE_SZ (50*sizeof(char))
char light_queue_buff[LIGHT_QUEUE_SZ];

void blink_state_change_task(void *pvParameter){
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1){
		if(xQueueReceive(light_queue, (void*)&light_queue_buff, portMAX_DELAY)){
			if(memcmp(&light_queue_buff, "0", sizeof("0")-1) == 0){
				ESP_LOGI(TAG, "Turning LED OFF");
				gpio_set_level(BLINK_GPIO, 0);
			}else{
				ESP_LOGI(TAG, "Turning LED ON");
				gpio_set_level(BLINK_GPIO, 1);
			}
		}
    }
}

void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        esp_mqtt_client_publish(mqtt_client, blink_pub_topic, "0",sizeof("0")-1, 1, 0);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        esp_mqtt_client_publish(mqtt_client, blink_pub_topic, "1",sizeof("1")-1, 1, 0);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void blink_start(){
	light_queue = xQueueCreate(10, LIGHT_QUEUE_SZ);
	vMqttWaitForConnection();
    vMqttSubscribe(blink_sub_topic, &light_queue);
    xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL);
    xTaskCreate(&blink_state_change_task, "blink_state_task", 2048, NULL, 5, NULL);
}

