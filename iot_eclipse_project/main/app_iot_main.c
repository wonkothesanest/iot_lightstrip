/*
 * TODO: License
 *
 * Main program for the IOT device to start out.
 *
 * Launches and configures system and then starts FreeRTOS
 *
 * */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "driver/wifi.h"
#include "driver/blink.h"
#include "driver/mqtt_test.h"
#include "driver/pwm.h"
#include "sdkconfig.h"
//#include "FreeRTOSConfig.h"

#define BLINK_GPIO 2


void app_main()
{
    printf("Hello world!\n");

    vWifiStart();
    vMqttStart();

    vPwmStart();


    //blink_start();


    for(;;){
    	vTaskDelay(10000/portTICK_PERIOD_MS);
    }

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
