/*
 * MqttTest.cpp
 *
 *  Created on: Jul 24, 2018
 *      Author: dustin
 */

#include "wifi.h"
#include "mqtt_client.h"
#include "mqtt_config.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"



#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "mqtt_test.h"
#include "sdkconfig.h"


static const char *TAG = "mqtt";
const char * sub_topic = "/home/mqtt/sub";
const char * pub_topic = "/home/mqtt/pub";

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
            ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}

void start_mqtt(){
	vWaitForWifiConnection();

	const esp_mqtt_client_config_t mqtt_cfg = {
	    .uri = "mqtt://192.168.1.18",
		.port = 1883,
	    .event_handle = mqtt_event_handler
	    // .user_context = (void *)your_context
	};
	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(client);
	esp_mqtt_client_subscribe(client, sub_topic, 1);
	const char * data = "hey hey what you say";
	for(;;){
		vTaskDelay(1000/portTICK_PERIOD_MS);
		esp_mqtt_client_publish(client, pub_topic, data, strlen(data), 1, 1);
	}

}
void subscribeAndPrint() {

}
void publish(const char * msg){

}
