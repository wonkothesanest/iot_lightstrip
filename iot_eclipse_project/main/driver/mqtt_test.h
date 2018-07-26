/*
 * MqttTest.h
 *
 *  Created on: Jul 24, 2018
 *      Author: dustin
 */

#ifndef MAIN_DRIVER_MQTT_TEST_H_
#define MAIN_DRIVER_MQTT_TEST_H_
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "mqtt_client.h"
#include "mqtt_config.h"

#define MQTT_QUEUE_PTR_ARR_SZ 10
/**
 * General Definition of usage of mqtt publish and most
 * importantly subscribe...
 *
 * You must create your own queue that you are going to wait on
 * then when the time comes to subscribe to a topic you may
 * call vMqttSubscribe with a handle to the queue, this queue
 * must be big enough to carry the data back to your function
 *
 * Then a task should wait on a queue state change and pull information
 * from the queue when available.
 *
 * See blink.h, blink.c for examples
 *
 */
/**
 * Struct to hold a topic and a queue to notify
 * when that topic is recieved
 */
struct mqtt_sub_queue {
	const char * topic;
	const QueueHandle_t * evt_queue;
};

/**
 * An extendable context for use with the Mqtt client
 */
struct mqtt_user_context {
	int * queue_sub_ind_ptr;
};

void vMqttStart();
EventGroupHandle_t mqtt_connected_event_group;
esp_mqtt_client_handle_t mqtt_client;
struct mqtt_sub_queue mqtt_queue_arr[MQTT_QUEUE_PTR_ARR_SZ];
//TODO: implement
void vMqttSubscribe(const char * topic, QueueHandle_t * evt_queue);
void vMqttPublish(const char * topic, const char * msg, int msg_len);
void vMqttWaitForConnection();
//
#endif /* MAIN_DRIVER_MQTT_TEST_H_ */
