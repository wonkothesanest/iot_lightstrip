/*
 * wifi.h
 *
 *  Created on: Jul 23, 2018
 *      Author: dustin
 */

#ifndef MAIN_DRIVER_WIFI_H_
#define MAIN_DRIVER_WIFI_H_

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"


#define EXAMPLE_ESP_WIFI_MODE_AP   CONFIG_ESP_WIFI_MODE_AP //TRUE:AP FALSE:STA
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_MAX_STA_CONN       CONFIG_MAX_STA_CONN

/*
#define EXAMPLE_ESP_WIFI_MODE_AP   false //TRUE:AP FALSE:STA
#define EXAMPLE_ESP_WIFI_SSID      "Steven Universe"
#define EXAMPLE_ESP_WIFI_PASS      "GemsRock"
#define EXAMPLE_MAX_STA_CONN       4 //CONFIG_MAX_STA_CONN
*/



void wifi_start();

#endif /* MAIN_DRIVER_WIFI_H_ */
