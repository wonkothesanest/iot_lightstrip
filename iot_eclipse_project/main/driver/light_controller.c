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
#include "util.h"
#include "config.h"
#include "animation.h"

static const char * TAG = "Light Controller";

QueueHandle_t hsv_light_queue;
QueueHandle_t anim_light_queue;
char hsv_light_queue_buff[HSV_LIGHT_QUEUE_SZ];
char anim_light_queue_buff[HSV_LIGHT_QUEUE_SZ];


static int hsv_parse_csv(int * ret, char * buff){
	//grab the 3 values
	char * c1 = strchr(buff, ',');
	if(c1 == NULL){
		return -3;
	}
	*c1 = 0;
	c1++;
	char * c2 = strchr(c1, ',');
	if(c2 != NULL){
		*c2 = 0;
		c2++;
		if(str2int(&ret[0], (char*)buff, 10)!= STR2INT_SUCCESS ||
			str2int(&ret[1], c1, 10) != STR2INT_SUCCESS||
			str2int(&ret[2], c2, 10) != STR2INT_SUCCESS ||
			ret[0] > 360 ||
			ret[1] > 100 ||
			ret[2] > 100){
			ESP_LOGE(TAG, "Could not convert string to HSV value");
			return -2;
		}else{
			return 0;
		}
	}else{
		ESP_LOGE(TAG, "Found String without 2 commas");
		return -1;
	}
}

void hsv_cmd_task(void *pvParameter){
	struct HSV hsv = {
		.h = 0.0,
		.s = 0.0,
		.v = 1.0
	};
    while(1){
		if(xQueueReceive(hsv_light_queue, (void*)&hsv_light_queue_buff, portMAX_DELAY)){
			//switch on various commands like up and down and hsv value
			//TODO: check to make sure the assignment of a null char worked
			ESP_LOGI(TAG, "Received an item from the queue");
			if(memcmp(&hsv_light_queue_buff, "OFF", sizeof("OFF")) == 0){
				ESP_LOGI(TAG, "Turning Lights OFF");
				vPWMTurnOff();
			}else if(memcmp(&hsv_light_queue_buff, "ON", sizeof("ON")) == 0){
				ESP_LOGI(TAG, "Turning Lights ON");
				vPWMTurnOn();
			}else if(strchr((char*)&hsv_light_queue_buff, ',')!=NULL){
				int hsvArr[3];
				if(hsv_parse_csv((int*)&hsvArr, (char*)&hsv_light_queue_buff)==0){
					//make a parse to get hsv value
					// 0-100 for S&V to double 0.0-1.0
					hsv.h = (double)hsvArr[0];
					hsv.s = (double)hsvArr[1]/100.0;
					hsv.v = (double)hsvArr[2]/100.0;
					ESP_LOGI(TAG, "Setting HSV %d, %d, %d", hsvArr[0], hsvArr[1], hsvArr[2]);
					vPwmSetValue(hsv);

				}else{
					ESP_LOGE(TAG, "Could not parse out integers from data [%s] error code [%d]", (char*)&hsv_light_queue_buff, hsv_parse_csv((int*)&hsvArr, (char*)&hsv_light_queue_buff));
				}

			}else{
				//TODO: dim up, dim down
				ESP_LOGE(TAG, "Command Not Found [%s]", hsv_light_queue_buff);
			}
		}else{
			ESP_LOGE(TAG, "Queue was not able to receive");
		}
    }
}

void anim_cmd_task(void *pvParameter){
    while(1){
		if(xQueueReceive(anim_light_queue, (void*)&anim_light_queue_buff, portMAX_DELAY)){
			ESP_LOGI(TAG, "Received an item from the animation queue");
			if(memcmp(&anim_light_queue_buff, ANIM_CMD_FADE, sizeof(ANIM_CMD_FADE)) == 0){
				ESP_LOGI(TAG, "Fading lights to OFF");
				vAnimationFade(2000);
			}else if(memcmp(&anim_light_queue_buff, ANIM_CMD_PULSE, sizeof(ANIM_CMD_PULSE)) == 0){
				ESP_LOGI(TAG, "Pulsing");
				vAnimationPulseCnt(200, 200, 3);
			}else if(memcmp(&anim_light_queue_buff, ANIM_CMD_FLASH, sizeof(ANIM_CMD_FLASH)) == 0){
				ESP_LOGI(TAG, "Flashing");
				vAnimationFlashCnt(200, 200, 3);
			}else if(memcmp(&anim_light_queue_buff, ANIM_CMD_RANDOM, sizeof(ANIM_CMD_RANDOM)) == 0){
				ESP_LOGI(TAG, "Random Setting");
				vAnimationRandom(3000);
			}else{
				//TODO: dim up, dim down
				ESP_LOGE(TAG, "Command Not Found [%s]", anim_light_queue_buff);
			}
		}else{
			ESP_LOGE(TAG, "Queue was not able to receive");
		}

    }
}



void vStartLightController() {

	hsv_light_queue = xQueueCreate(10, HSV_LIGHT_QUEUE_SZ);
	anim_light_queue = xQueueCreate(10, HSV_LIGHT_QUEUE_SZ);
	vMqttWaitForConnection();
    vMqttSubscribe(MQTT_TOPIC_HSV_CMD, &hsv_light_queue);
    vMqttSubscribe(MQTT_TOPIC_HSV_ANIM, &anim_light_queue);
    xTaskCreate(&hsv_cmd_task, "hsv_task", 2048, NULL, 5, NULL);
    xTaskCreate(&anim_cmd_task, "anim_task", 2048, NULL, 5, NULL);

}

