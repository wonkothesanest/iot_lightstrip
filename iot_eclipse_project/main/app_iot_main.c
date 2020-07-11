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
#include "esp_log.h"

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
#include "driver/iot_system.h"
#include "driver/udp_logging/include/udp_logging.h"
//#include "FreeRTOSConfig.h"

#define BLINK_GPIO 2

static const char *TAG = "main";

void app_main()
{
	ESP_LOGI(TAG, "Application Main is Starting up");
	ESP_LOGI(TAG, "App: configQUEUE_REGISTRY_SIZE:        %d", configQUEUE_REGISTRY_SIZE );
//	ESP_LOGI(TAG, "App: configTOTAL_HEAP_SIZE:            %d", configTOTAL_HEAP_SIZE );
	ESP_LOGI(TAG, "App: configISR_STACK_SIZE:             %d", configISR_STACK_SIZE );
	ESP_LOGI(TAG, "App: configMINIMAL_STACK_SIZE:         %d", configMINIMAL_STACK_SIZE );
	ESP_LOGI(TAG, "App: configTICK_RATE_HZ:               %d", configTICK_RATE_HZ );
	ESP_LOGI(TAG, "App: configCHECK_FOR_STACK_OVERFLOW:   %d", configCHECK_FOR_STACK_OVERFLOW );

	ESP_LOGI(TAG, "PWM");
    vPwmStart();
	ESP_LOGI(TAG, "GPIO");
    vGPIOInterruptStart();
	ESP_LOGI(TAG, "WIFI");
    vWifiStart();
	ESP_LOGI(TAG, "OTA");
    vOtaStart();
	ESP_LOGI(TAG, "MQTT");
    vMqttStart();
	ESP_LOGI(TAG, "LIGHT");
    vStartLightController();
	ESP_LOGI(TAG, "PIR");
    vPirStart();
	ESP_LOGI(TAG, "ANIM");
    vAnimationStart();
	ESP_LOGI(TAG, "SYSTEM");
    vSystemStart();
	ESP_LOGI(TAG, "UDP");
    udp_logging_init(CONNECTION_LAPTOP_IP, 1337);


    //blink_start();

    ESP_LOGI(TAG, "Application has started");



    for(;;){
    	vTaskDelay(portMAX_DELAY);
    }

    ESP_LOGI(TAG, "Application Is Restarting");

    fflush(stdout);
    esp_restart();
}
