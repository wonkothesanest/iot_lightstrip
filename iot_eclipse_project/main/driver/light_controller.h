/*
 * light_controller.h
 *
 *  Created on: Jul 26, 2018
 *      Author: dustin
 */

#ifndef MAIN_DRIVER_LIGHT_CONTROLLER_H_
#define MAIN_DRIVER_LIGHT_CONTROLLER_H_

#include <string.h>
#define HSV_LIGHT_QUEUE_SZ (50*sizeof(char))

const char * mqtt_topic_hsv_cmd = "home/kitchen/cKitchen_Cabinet_Lights/command";
//const char * mqtt_topic_hsv_state = "";
void vStartLightController();




#endif /* MAIN_DRIVER_LIGHT_CONTROLLER_H_ */
