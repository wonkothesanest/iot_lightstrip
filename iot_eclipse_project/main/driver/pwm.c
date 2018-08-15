/*
 * blink.c
 *
 *  Created on: Jul 23, 2018
 *      Author: dustin
 */
#include "blink.h"
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "sdkconfig.h"
#include "driver/ledc.h"
#include "pwm.h"
#include "esp_log.h"
#include "config.h"

#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_HS_CH_R_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH_G_CHANNEL    LEDC_CHANNEL_1
#define LEDC_HS_CH_B_CHANNEL    LEDC_CHANNEL_2

#define LEDC_TEST_DUTY         (4000)
#define LEDC_TEST_FADE_TIME    (800)

#define LEDC_DUTY_MAX (4092-1)

struct HSV hsv_last_value = {
	.h = 0.0,
	.s = 0.0,
	.v = 0.0
};
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
enum pwm_colors {
	PWM_C_RED = 0,
	PWM_C_GREEN = 1,
	PWM_C_BLUE = 2
};
ledc_channel_config_t ledc_channel[3] ={
		{
				.channel = LEDC_HS_CH_R_CHANNEL,
				.duty = 0,
				.gpio_num = LEDC_HS_CH_R_GPIO,
				.speed_mode = LEDC_HS_MODE,
				.timer_sel = LEDC_HS_TIMER
		},
		{
				.channel = LEDC_HS_CH_G_CHANNEL,
				.duty = 0,
				.gpio_num = LEDC_HS_CH_G_GPIO,
				.speed_mode = LEDC_HS_MODE,
				.timer_sel = LEDC_HS_TIMER
		},
		{
				.channel = LEDC_HS_CH_B_CHANNEL,
				.duty = 0,
				.gpio_num = LEDC_HS_CH_B_GPIO,
				.speed_mode = LEDC_HS_MODE,
				.timer_sel = LEDC_HS_TIMER
		}
};

bool brighten = true;
static const char * TAG = "PWM";

void pwm_task(void *pvParameter) {
	struct HSV hsv = {
			.h = 0.0,
			.s = 0.0,
			.v = 0.0
	};
	struct HSV hsv2;
	while(1){
		//if beyond the edge
		if(hsv.v > 1.0){
			hsv.v = 0.0;
		}

		if(brighten){
			vPwmSetValue(hsv);
			ESP_LOGI(TAG, "Setting Brightness %d", (uint8_t)(hsv.v*100.0));
		}else{
			memcpy(&hsv2, &hsv, sizeof(hsv));
			hsv2.v = 0.0;
			vPwmSetValue(hsv2);
		}
		brighten = !brighten;
		hsv.v += 0.1;
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
			.freq_hz = 500,                      // frequency of PWM signal
			.speed_mode = LEDC_HS_MODE,           // timer mode
			.timer_num = LEDC_HS_TIMER            // timer index
			};
	// Set configuration of timer0 for high speed channels
	ledc_timer_config(&ledc_timer);

	// Set LED Controller with previously prepared configuration
	ledc_channel_config(&ledc_channel[PWM_C_RED]);
	ledc_channel_config(&ledc_channel[PWM_C_GREEN]);
	ledc_channel_config(&ledc_channel[PWM_C_BLUE]);

	// Initialize fade service.
	ledc_fade_func_install(0);

	//xTaskCreate(&pwm_task, "PWMTask", 2048, NULL, 5, NULL);
}

static uint32_t uiPwmConvertPercent(double v){
	uint32_t duty = 0;
	if(v == 0.0){
		duty = 0;
	}else if(v >= 1.0){
		duty = LEDC_DUTY_MAX;
	}else{
		duty = LEDC_DUTY_MAX*v/1.0;
	}
	return duty;
}
static double uiPwmConvertDutyToPercent(uint32_t duty){
	double percent = 0;
	if( duty == 0){
		percent = 0;
	}else if(duty == LEDC_DUTY_MAX){
		percent = 1.0;
	}else{
		percent = (double)1.0*(double)duty/(double)LEDC_DUTY_MAX;
	}
	return percent;
}


//TODO: send state back to MQTT values
void vPwmSetValue(struct HSV hsv) {
	//Convert to usefull rgb
	struct RGB rgb = xHSVToRGB(hsv);
	//Store last set value for on off
	memcpy(&hsv_last_value, &hsv, sizeof(hsv_last_value));

	//convert 0-100 to 0-4092 (12 bit signal)
	uint32_t duty_r = uiPwmConvertPercent(rgb.r);
	uint32_t duty_g = uiPwmConvertPercent(rgb.g);
	uint32_t duty_b = uiPwmConvertPercent(rgb.b);
	ESP_LOGI(TAG, "Setting H,S,V values [%lf, %lf, %lf] for duty cycles [%d, %d, %d]",hsv.h, hsv.s, hsv.v, duty_r, duty_g, duty_b);

	ledc_set_fade_with_time(ledc_channel[PWM_C_RED].speed_mode, ledc_channel[PWM_C_RED].channel,
			duty_r, LEDC_TEST_FADE_TIME);
	ledc_set_fade_with_time(ledc_channel[PWM_C_GREEN].speed_mode, ledc_channel[PWM_C_GREEN].channel,
			duty_g, LEDC_TEST_FADE_TIME);
	ledc_set_fade_with_time(ledc_channel[PWM_C_BLUE].speed_mode, ledc_channel[PWM_C_BLUE].channel,
			duty_b, LEDC_TEST_FADE_TIME);

	ledc_fade_start(ledc_channel[PWM_C_RED].speed_mode, ledc_channel[PWM_C_RED].channel,
			LEDC_FADE_NO_WAIT);
	ledc_fade_start(ledc_channel[PWM_C_GREEN].speed_mode, ledc_channel[PWM_C_GREEN].channel,
			LEDC_FADE_NO_WAIT);
	ledc_fade_start(ledc_channel[PWM_C_BLUE].speed_mode, ledc_channel[PWM_C_BLUE].channel,
			LEDC_FADE_NO_WAIT);
}


struct HSV iPwmGetValue() {
	double r = uiPwmConvertDutyToPercent(ledc_get_duty(ledc_channel[PWM_C_RED].speed_mode, ledc_channel[PWM_C_RED].channel));
	double g = uiPwmConvertDutyToPercent(ledc_get_duty(ledc_channel[PWM_C_GREEN].speed_mode, ledc_channel[PWM_C_GREEN].channel));
	double b = uiPwmConvertDutyToPercent(ledc_get_duty(ledc_channel[PWM_C_BLUE].speed_mode, ledc_channel[PWM_C_BLUE].channel));
	struct RGB rgb = {
			.r = r,
			.g = g,
			.b = b
	};
	return xRGBToHSV(rgb);

}

void vPWMTurnOff(){
	struct HSV hsv = {
			.h = 0.0,
			.s = 0.0,
			.v = 0.0
	};
	vPwmSetValue(hsv);
}

//TODO: remember last state
void vPWMTurnOn(){
	vPwmSetValue(hsv_last_value);
}

