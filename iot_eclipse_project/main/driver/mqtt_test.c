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

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xEventGroupSetBits(mqtt_connected_event_group, BIT0);
            msg_id = esp_mqtt_client_subscribe(client, sub_topic, 1);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
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
            //TODO: make an event registry for each of the subscribed topics
            if(memcmp(event->topic, sub_topic, strlen(sub_topic)-1)== 0){
            	if(memcmp(event->data, "0", sizeof("0")-1) == 0){
                    ESP_LOGI(TAG, "Turning LED OFF");
                    gpio_set_level(2, 0);

            	}else{
                    ESP_LOGI(TAG, "Turning LED ON");
                    gpio_set_level(2, 1);
            	}
            }

            printf("TOPIC=%.*s [%d]\r\n", event->topic_len, event->topic, event->topic_len);
            printf("SUBTOPIC=%.*s [%d] [%d]\r\n", strlen(sub_topic), sub_topic, memcmp(event->topic, sub_topic, strlen(sub_topic)-1), strlen(sub_topic));
            printf("DATA=%.*s [%d]\r\n", event->data_len, event->data, event->data_len);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}

void vMqttStart(){

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
	    .event_handle = mqtt_event_handler//,
		//.transport = MQTT_TRANSPORT_OVER_TCP
	    // .user_context = (void *)your_context
	};
	mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(mqtt_client);

}
void vMqttWaitForConnection(){
	xEventGroupWaitBits(mqtt_connected_event_group, BIT0, false, true, portMAX_DELAY);
}


