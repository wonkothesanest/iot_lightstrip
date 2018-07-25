/*
 * MqttTest.h
 *
 *  Created on: Jul 24, 2018
 *      Author: dustin
 */

#ifndef MAIN_DRIVER_MQTT_TEST_H_
#define MAIN_DRIVER_MQTT_TEST_H_
#include "freertos/event_groups.h"
#include "mqtt_client.h"
#include "mqtt_config.h"

void vMqttStart();
EventGroupHandle_t mqtt_connected_event_group;
esp_mqtt_client_handle_t mqtt_client;
//TODO: implement
void subscribe(const char * topic);
void publish(const char * topic, const char * msg);
void vMqttWaitForConnection();
//
#endif /* MAIN_DRIVER_MQTT_TEST_H_ */
