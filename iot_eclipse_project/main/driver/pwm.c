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
#include "driver/ledc.h"
#include "pwm.h"
#include "esp_log.h"

#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (2)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0

#define LEDC_TEST_DUTY         (4000)
#define LEDC_TEST_FADE_TIME    (800)

#define LEDC_DUTY_MAX (4092-1)
/*
 * Prepare individual configuration
 * for each channel of LED Controller
 * by selecting:
 * - controller's channel number
 * - output duty cycle, set initially to 0
 * - GPIO number where LED is connected to
 * - speed mode, either high or low
 * - timer servicing selected channel
 *   Note: if different channels use one timer,
 *         then frequency and bit_num of these channels
 *         will be the same
 */
ledc_channel_config_t ledc_channel = {
		.channel = LEDC_HS_CH0_CHANNEL,
		.duty = 0,
		.gpio_num = LEDC_HS_CH0_GPIO,
		.speed_mode = LEDC_HS_MODE,
		.timer_sel = LEDC_HS_TIMER
};

bool brighten = true;
int brightness = 0;
static const char * TAG = "PWM";

void pwm_task(void *pvParameter) {
	while(1){
		if(brightness > 100){
			brightness = 0;
		}
		if(brighten){
			vPwmSetValue(brightness);
			ESP_LOGI(TAG, "Setting Brightness %d", brightness);
		}else{
			vPwmSetValue(0);
		}
		brighten = !brighten;
		brightness += 10;
		vTaskDelay(LEDC_TEST_FADE_TIME/portTICK_PERIOD_MS);
	}
}

void vPwmStart() {

	/*
	 * Prepare and set configuration of timers
	 * that will be used by LED Controller
	 */
	ledc_timer_config_t ledc_timer = {
			.duty_resolution = LEDC_TIMER_12_BIT, // resolution of PWM duty
			.freq_hz = 5000,                      // frequency of PWM signal
			.speed_mode = LEDC_HS_MODE,           // timer mode
			.timer_num = LEDC_HS_TIMER            // timer index
			};
	// Set configuration of timer0 for high speed channels
	ledc_timer_config(&ledc_timer);

	// Set LED Controller with previously prepared configuration
	ledc_channel_config(&ledc_channel);

	// Initialize fade service.
	ledc_fade_func_install(0);

	xTaskCreate(&pwm_task, "PWMTask", 2048, NULL, 5, NULL);
}

static uint32_t uiPwmConvertPercent(int v){
	uint32_t duty = 0;
	if(v == 0){
		duty = 0;
	}else if(v == 100){
		duty = LEDC_DUTY_MAX;
	}else{
		duty = LEDC_DUTY_MAX*v/100;
	}
	return duty;
}
static int uiPwmConvertDutyToPercent(uint32_t duty){
	int percent = 0;
	if( duty == 0){
		percent = 0;
	}else if(duty == LEDC_DUTY_MAX){
		percent = 100;
	}else{
		percent = 100*duty/LEDC_DUTY_MAX;
	}
	return percent;
}

void vPwmSetValue(int v) {
	//convert 0-100 to 0-4092 (12 bit signal)
	uint32_t duty = uiPwmConvertPercent(v);
	ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel,
			duty, LEDC_TEST_FADE_TIME);
	ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel,
			LEDC_FADE_NO_WAIT);
}



uint32_t iPwmGetDutyValue() {
	return ledc_get_duty(ledc_channel.speed_mode, ledc_channel.channel);
}
int iPwmGetValue() {
	return uiPwmConvertDutyToPercent(ledc_get_duty(ledc_channel.speed_mode, ledc_channel.channel));
}
