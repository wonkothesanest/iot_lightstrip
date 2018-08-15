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

/**
 * Pin definitions
 *  all pins should be different
 */
#define GPIO_INPUT_IO_PIR     		(4)
#define GPIO_INPUT_IO_RNG_FINDER	(19)
#define LEDC_HS_CH_R_GPIO       	(13)
#define LEDC_HS_CH_G_GPIO       	(14)
#define LEDC_HS_CH_B_GPIO       	(26)

//URI setup for MQTT based commands
#define MQTT_BASE_URI 				"home/kitchen/"
#define MQTT_PIR     				MQTT_BASE_URI "sPIR/"
#define MQTT_LIGHTS  				MQTT_BASE_URI "cKitchen_Cabinet_Lights/"
#define MQTT_IR  					MQTT_BASE_URI "strIR/"
#define MQTT_RANGE  				MQTT_BASE_URI "iRange/"

#define MQTT_TOPIC_HSV_CMD			MQTT_LIGHTS "command"
#define MQTT_TOPIC_HSV_STATUS		MQTT_LIGHTS "status"
#define MQTT_TOPIC_HSV_ANIM			MQTT_LIGHTS "animation"
#define MQTT_TOPIC_PIR_CMD			MQTT_PIR "command"
/*
#define MQTT_TOPIC_IR_CMD			MQTT_IR "command"
#define MQTT_TOPIC_RANGE_STATUS		MQTT_RANGE "status"
*/



#endif /* CONFIG_CONFIG_H_ */
