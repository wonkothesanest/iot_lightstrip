/*
 * animation.c
 *
 *  Created on: Aug 14, 2018
 *      Author: dustin
 */
#include "animation.h"
#include "HSVToRGB.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
//#include "freertos/queue.h"
#include "pwm.h"
#include "esp_log.h"

static const char * TAG = "ANIM";

struct HSV origValue;
struct HSV curValue;
uint8_t anim_reg = 0;
int R1 = 0;
int R2 = 0;
int R3 = 0;
double min_val = 0.0;
double max_val = 1.0;
SemaphoreHandle_t animation_semaphore;

void vAnimationLoop(void * pvParameters){
	bool flash_on = true;
	bool fade_in = true;
	int delayTime = 0;
	while(1){
		if(anim_reg == 0){
			vTaskDelay(100/portTICK_PERIOD_MS);
			continue;
		}
		xSemaphoreTake(animation_semaphore, 10/portTICK_PERIOD_MS);

		switch(anim_reg){
		//Fade
		case 1<<0:
				//100 steps per R1 ms
				curValue.v -= 0.01;
				if(curValue.v <= 0.0){
					curValue.v = 0.0;
				}
				vPwmSetValueRec(curValue, false, false);
				delayTime = (R1/100)/portTICK_PERIOD_MS;
				break;
		//Flash
		case 1<<1:
				if(flash_on){
					curValue.v = 0.0;
					vPwmSetValueRec(curValue, false, false);
					delayTime = R1/portTICK_PERIOD_MS;
				}else{
					curValue.v = origValue.v;
					vPwmSetValueRec(curValue, false, false);
					delayTime = R2/portTICK_PERIOD_MS;
					if(R3 != -1 && R3 > 0){
						R3--;
					}
				}
				flash_on = !flash_on;
				if(R3 == 0){
					flash_on = true;
					vAnimationStop();
				}
				break;
		//Pulse
		case 1<<2:
				if(fade_in){
					curValue.v += 0.01;
					if(curValue.v > max_val){
						curValue.v = max_val;
						fade_in = !fade_in;
					}
					delayTime = (R1/100)/portTICK_PERIOD_MS;
				}else{
					curValue.v -= 0.01;
					if(curValue.v < min_val){
						curValue.v = min_val;
						fade_in = !fade_in;
						if(R3>0){
							R3--;
						}
					}
					delayTime = (R2/100)/portTICK_PERIOD_MS;
				}
				vPwmSetValueRec(curValue, false, false);
				if(R3 == 0){
					vAnimationStop();
				}
				break;
		//Random
		case 1<<3:
				curValue.h = rand() % 360;
				curValue.s = (double)(rand() % 100)/100.0;
				vPwmSetValueRec(curValue, false, false);
				delayTime = R1/portTICK_PERIOD_MS;
				break;
		}

		//Release and take mutex / semiphore
		xSemaphoreGive(animation_semaphore);
		vTaskDelay(delayTime);
	}
}

bool bAnimationRunning(){
	return anim_reg != 0;
}

void vAnimationStart() {
	anim_reg = 0;
	animation_semaphore = xSemaphoreCreateBinary();
    xTaskCreate(&vAnimationLoop, "animation_task", 2048, NULL, 5, NULL);
}

void vAnimationStop() {
	xSemaphoreTake(animation_semaphore, 100/portTICK_PERIOD_MS);
	anim_reg = 0;
	vPwmSetValue(origValue);
	xSemaphoreGive(animation_semaphore);
}
void vAnimationSetup(uint8_t anim, int r1, int r2, int r3){
	origValue = iPwmGetValue();
	curValue = iPwmGetValue();
	if(!xSemaphoreTake(animation_semaphore, 100/portTICK_PERIOD_MS)){
		ESP_LOGE(TAG, "Could not Take the Semaphore");
	}
	anim_reg = anim;
	R1 = r1;
	R2 = r2;
	R3 = r3;
	xSemaphoreGive(animation_semaphore);
}

void vAnimationFade(int ms) {
	vAnimationSetup(1<<0, ms, -1, -1);
}

void vAnimationFlash(int onMs, int offMs) {
	vAnimationSetup(1<<1, onMs, offMs, -1);
}

void vAnimationFlashCnt(int onMs, int offMs, int count) {
	vAnimationSetup(1<<1, onMs, offMs, count);
}

void vAnimationPulse(int onMs, int offMs) {
	vAnimationSetup(1<<2, onMs, offMs, -1);
}

void vAnimationPulseCnt(int onMs, int offMs, int count) {
	vAnimationSetup(1<<2, onMs, offMs, count);
}

void vAnimationRandom(int change_ms) {
	vAnimationSetup(1<<3, change_ms, -1, -1);
}
