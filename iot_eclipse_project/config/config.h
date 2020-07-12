/*
 * config.h
 *
 *  Created on: Jul 24, 2018
 *      Author: dustin
 */

#ifndef CONFIG_CONFIG_H_
#define CONFIG_CONFIG_H_
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

//Bits for flags and events
#define BIT0 0x00000001
#define BIT1 0x00000002
#define BIT2 0x00000004

#define CONFIG_LOG_DEFAULT_LEVEL 3

#define LOCATION_KITCHEN_CABINET 1
#define LOCATION_BED 2
#define LOCATION_BOOK_SHELF 3

// Define which location this will be compiled for
// This is defined in project settings under C/C++ Build->Environment
// then uses the make file to supply the variables (best we can do)
// #define LOCATION       LOCATION_BOOK_SHELF

/**
 * Pin definitions
 *  all pins should be different
 */
#define GPIO_INPUT_IO_PIR     		(4)
#define GPIO_INPUT_IO_RNG_FINDER	(19)
#define LEDC_HS_CH_R_GPIO       	(14)
#define LEDC_HS_CH_G_GPIO       	(13)
#define LEDC_HS_CH_B_GPIO       	(26)

/**
 * PIR definitions
 * PIR_WAIT_BETWEEN_SIGNALS_MS sets the time after a signal is
 * triggered to wait until it will clear the bits and be able
 * to read another trigger.
 */
#define PIR_WAIT_BETWEEN_SIGNALS_MS 10000

//URI setup for MQTT based commands
//#ifdef LOCATION
//#error "hey you got somewhere " LOCATION
//#endif
#define OPENHAB_SERVER_NAME "raspberrypi"
#define CONNECTION_FIRMWARE_SERVER "https://raspberrypi:8000/"
#if LOCATION == LOCATION_KITCHEN_CABINET
#define MQTT_BASE_URI 				"home/kitchen/"
#define MQTT_LIGHTS  				MQTT_BASE_URI "cKitchen_Cabinet_Lights/"
#define FIRMWARE_UPGRADE_URL		CONNECTION_FIRMWARE_SERVER "iot-kitchen.bin"
#define DEVICE_NAME 				"iot-kitchen"
#elif LOCATION == LOCATION_BED
#define MQTT_BASE_URI 				"home/livingroom/"
#define MQTT_LIGHTS  				MQTT_BASE_URI "cBed_Lights/"
#define FIRMWARE_UPGRADE_URL		CONNECTION_FIRMWARE_SERVER  "iot-bed-windows.bin"
#define DEVICE_NAME 				"iot-bed-windows"
#elif LOCATION == LOCATION_BOOK_SHELF
#define MQTT_BASE_URI 				"home/livingroom/"
#define MQTT_LIGHTS  				MQTT_BASE_URI "cBook_Shelf_Lights/"
#define FIRMWARE_UPGRADE_URL		CONNECTION_FIRMWARE_SERVER  "iot-bookshelf.bin"
#define DEVICE_NAME 				"iot-bookshelf"
#else
#define MQTT_BASE_URI 				"home/room/"
#define MQTT_LIGHTS  				MQTT_BASE_URI "lights/"
#define FIRMWARE_UPGRADE_URL		CONNECTION_FIRMWARE_SERVER "iot-identification.bin"
#define DEVICE_NAME 				"DNS-Name"

#error "LOCATION must be defined"
#endif

#define MQTT_PIR     				MQTT_BASE_URI "sPIR/"
#define MQTT_IR  					MQTT_BASE_URI "strIR/"
#define MQTT_RANGE  				MQTT_BASE_URI "iRange/"
#define MQTT_RESTART_STATUS			MQTT_BASE_URI "restart/status"

#define MQTT_TOPIC_HSV_CMD			MQTT_LIGHTS "command"
#define MQTT_TOPIC_HSV_STATUS		MQTT_LIGHTS "status"
#define MQTT_TOPIC_HSV_ANIM			MQTT_LIGHTS "animation"
#define MQTT_TOPIC_PIR_CMD			MQTT_PIR "state"
#define MQTT_TOPIC_RES_CMD          MQTT_LIGHTS "restart"
/*
 #define MQTT_TOPIC_IR_CMD			MQTT_IR "command"
 #define MQTT_TOPIC_RANGE_STATUS		MQTT_RANGE "status"
 */

#define ANIM_CMD_NORMAL "STOP"
#define ANIM_CMD_FADE "FADE"
#define ANIM_CMD_PULSE "PULSE"
#define ANIM_CMD_FLASH "FLASH"
#define ANIM_CMD_RANDOM "RANDOM"

#endif /* CONFIG_CONFIG_H_ */
