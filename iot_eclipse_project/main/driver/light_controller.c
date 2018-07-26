/*
 * light_controller.c
 *
 *  Created on: Jul 26, 2018
 *      Author: dustin
 */

#include "light_controller.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "mqtt_test.h"
#include "pwm.h"
static const char * TAG = "Blink Example";
QueueHandle_t hsv_light_queue;
char hsv_light_queue_buff[HSV_LIGHT_QUEUE_SZ];


void hsv_cmd_task(void *pvParameter){
	struct HSV hsv = {
		.h = 0.0,
		.s = 0.0,
		.v = 1.0
	};
    while(1){
		if(xQueueReceive(hsv_light_queue, (void*)&hsv_light_queue_buff, portMAX_DELAY)){
			//switch on various commands like up and down and hsv value
			if(memcmp(&hsv_light_queue_buff, "OFF", sizeof("OFF")-1) == 0){
				ESP_LOGI(TAG, "Turning Lights OFF");
				vPWMTurnOff();
			}else if(memcmp(&hsv_light_queue_buff, "ON", sizeof("ON")-1) == 0){
				ESP_LOGI(TAG, "Turning Lights ON");
				vPWMTurnOn();
			}else if(false){
				//make a parse to get hsv value
				vPwmSetValue(hsv);
			}else{
				//TODO: dim up, dim down
				ESP_LOGE(TAG, "Command Not Found [%s]", hsv_light_queue_buff);
			}
		}
    }
}


void vStartLightController() {

	hsv_light_queue = xQueueCreate(10, HSV_LIGHT_QUEUE_SZ);
	vMqttWaitForConnection();
    vMqttSubscribe(mqtt_topic_hsv_cmd, &hsv_light_queue);
    xTaskCreate(&hsv_cmd_task, "hsv_task", 2048, NULL, 5, NULL);

}

