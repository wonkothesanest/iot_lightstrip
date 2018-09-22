/*
 * TODO: License
 *
 * Main program for the IOT device to start out.
 *
 * Launches and configures system and then starts FreeRTOS
 *
 * */
#include <stdio.h>
#include "FreeRTOSConfig.h"
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
#include "driver/gpio_interrupts.h"
#include "driver/light_controller.h"
#include "driver/pir.h"
#include "driver/animation.h"
#include "driver/ota.h"
//#include "FreeRTOSConfig.h"

#define BLINK_GPIO 2


void app_main()
{
    printf("Hello world!\n");

    vPwmStart();
    vGPIOInterruptStart();
    vWifiStart();
    vOtaStart();
    vMqttStart();
    vStartLightController();
    vPirStart();
    vAnimationStart();

    //blink_start();


    for(;;){
    	vTaskDelay(portMAX_DELAY);
    }

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
