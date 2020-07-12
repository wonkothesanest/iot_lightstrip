/*
 * iot_sys.c
 *
 *  Created on: Oct 26, 2018
 *      Author: dustin
 */
#include "esp_log.h"
#include "iot_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "mqtt_test.h"
#include "config.h"
#include "esp_system.h"

static const char * TAG = "IOT System";

QueueHandle_t iot_sys_restart_queue;
char iot_sys_restart_queue_buff[IOT_SYS_QUEUE_SZ];

void restart_cmd_task(void *pvParameter){
    while(1){
		if(xQueueReceive(iot_sys_restart_queue, (void*)&iot_sys_restart_queue_buff, portMAX_DELAY)){
			ESP_LOGI(TAG, "Received an item from the restart queue");
			ESP_LOGI(TAG, "Restarting now");
	        esp_restart();
		}else{
			ESP_LOGE(TAG, "Restart queue was not able to receive");
		}
    }
}



void vSystemStart() {
	ESP_LOGI(TAG, "Starting");
	iot_sys_restart_queue = xQueueCreate(10, IOT_SYS_QUEUE_SZ);
	vMqttWaitForConnection();
    vMqttSubscribe(MQTT_TOPIC_RES_CMD, &iot_sys_restart_queue);
    vMqttSubscribe(MQTT_TOPIC_ALL_RES_CMD, &iot_sys_restart_queue);
    xTaskCreate(&restart_cmd_task, "restart_listen_task", 2048, NULL, 5, NULL);
	ESP_LOGI(TAG, "Finished");
}

