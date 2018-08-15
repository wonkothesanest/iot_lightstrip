/*
 * MqttTest.cpp
 *
 *  Created on: Jul 24, 2018
 *      Author: dustin
 */

#include "wifi.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/projdefs.h"



#include "mqtt_test.h"
#include "sdkconfig.h"
#include "tcpip_adapter.h"
#include "config.h"


static const char *TAG = "mqtt";
const char * sub_topic = "/home/light/command";
const char * pub_topic = "/home/mqtt/pub";
int mqtt_queue_subscribe_index = 0;
struct mqtt_user_context mqtt_context;

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    //counter for looping messages
    int queue_ind_cntr = 0;
    //bool for finding the queue
    bool found_queue = false;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xEventGroupSetBits(mqtt_connected_event_group, BIT0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            xEventGroupClearBits( mqtt_connected_event_group, BIT0);
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            // for each message we get we need to find the correct queue to notify
            found_queue = false;
            ESP_LOGI(TAG, "Total number in queue pointers registered %d", *(int*)((struct mqtt_user_context *)event->user_context)->queue_sub_ind_ptr);
            for(queue_ind_cntr = 0; queue_ind_cntr < *(int*)((struct mqtt_user_context *)event->user_context)->queue_sub_ind_ptr; queue_ind_cntr++){
            	//ESP_LOGI(TAG, "Checking Queue topic [%s] against event topic [%s]", mqtt_queue_arr[queue_ind_cntr].topic, event->topic);
                if(memcmp(event->topic, mqtt_queue_arr[queue_ind_cntr].topic, event->topic_len)== 0){
                	//To catch the case of having /home/light and /home/light/state
                	if(mqtt_queue_arr[queue_ind_cntr].topic[event->topic_len]!= 0){
                		//TODO: Test this logic
                		ESP_LOGI(TAG, "Found a topic in a shorter form");
                		continue;
                	}else{
                		//we have the right topic now we copy over the data into the queue
                		// last character should be null for string termination
                		event->data[event->data_len] = '\0';
                		ESP_LOGI(TAG, "Found Topic [%s] appending data to its queue with length [%d]",mqtt_queue_arr[queue_ind_cntr].topic,event->data_len);
                		xQueueSendToBack(*(mqtt_queue_arr[queue_ind_cntr].evt_queue), (void*)event->data, 10);
                		found_queue = true;
                		break;
                	}
                }
            }
            if(!found_queue){
            	ESP_LOGE(TAG, "Did not find Queue topic subscribed to");
            }
            /*
            if(memcmp(event->topic, sub_topic, strlen(sub_topic)-1)== 0){
            	if(memcmp(event->data, "0", sizeof("0")-1) == 0){
                    ESP_LOGI(TAG, "Turning LED OFF");
                    gpio_set_level(2, 0);

            	}else{
                    ESP_LOGI(TAG, "Turning LED ON");
                    gpio_set_level(2, 1);
            	}
            }
            */

            printf("TOPIC=%.*s [%d]\r\n", event->topic_len, event->topic, event->topic_len);
            printf("DATA=%.*s [%d]\r\n", event->data_len, event->data, event->data_len);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}

void vMqttStart(){

	//Setup the user context
	mqtt_context.queue_sub_ind_ptr = &mqtt_queue_subscribe_index;

	//set up blink pin for state
    gpio_pad_select_gpio(2);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(2, GPIO_MODE_OUTPUT);

	mqtt_connected_event_group = xEventGroupCreate();
	vWifiWaitForConnection();
	ESP_LOGI(TAG, "Waiting for bits from openhab name");
	xEventGroupWaitBits(openhab_lookup_event_group, BIT0, false, true, portMAX_DELAY);
	ESP_LOGI(TAG, "Finished waiting for bits from openhab name");
	char addr[20];
	sprintf(&addr, "mqtt://%d.%d.%d.%d", IP2STR(&openhab_address));
	const esp_mqtt_client_config_t mqtt_cfg = {
		.uri = (const char *)&addr,
		.port = 1883,
	    .event_handle = mqtt_event_handler,
		.user_context = (void*)&mqtt_context
		//.transport = MQTT_TRANSPORT_OVER_TCP
	    // .user_context = (void *)your_context
	};
	mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(mqtt_client);

}
void vMqttWaitForConnection(){
	xEventGroupWaitBits(mqtt_connected_event_group, BIT0, false, true, portMAX_DELAY);
}

void vMqttSubscribe(const char * topic, QueueHandle_t * evt_queue){
    int msg_id = esp_mqtt_client_subscribe(mqtt_client, topic, 1);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
    if(mqtt_queue_subscribe_index >= MQTT_QUEUE_PTR_ARR_SZ - 1 ){
    	ESP_LOGE(TAG, "Queue pointer array size exceeded");
    	//Throw new exception
    	ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }
    mqtt_queue_arr[mqtt_queue_subscribe_index].topic = topic;
    mqtt_queue_arr[mqtt_queue_subscribe_index].evt_queue = evt_queue;
    ESP_LOGI(TAG, "Success Subscription to [%s] compared to [%s]", mqtt_queue_arr[mqtt_queue_subscribe_index].topic, topic);
    mqtt_queue_subscribe_index++;
}

void vMqttPublish(const char * topic, const char * msg, int msg_len){
    esp_mqtt_client_publish(mqtt_client, topic, msg, msg_len-1, 1, 0);
}



