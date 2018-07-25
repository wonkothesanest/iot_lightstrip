/*
 * blink.c
 *
 *  Created on: Jul 23, 2018
 *      Author: dustin
 */
#include "blink.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "sdkconfig.h"

#include "mqtt_test.h"
#include "mqtt_client.h"

const char * blink_pub_topic = "/home/light/state";

void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        esp_mqtt_client_publish(mqtt_client, blink_pub_topic, "0",sizeof("0")-1, 1, 0);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        esp_mqtt_client_publish(mqtt_client, blink_pub_topic, "1",sizeof("1")-1, 1, 0);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void blink_start(){
    xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL);

}

