/*
 * animation.h
 *
 *  Created on: Aug 14, 2018
 *      Author: dustin
 */

#ifndef MAIN_DRIVER_ANIMATION_H_
#define MAIN_DRIVER_ANIMATION_H_
#include <stdbool.h>

void vAnimationStart();
void vAnimationStop();
bool bAnimationRunning();

void vAnimationFade(int ms);
void vAnimationFlash(int onMs, int offMs);
void vAnimationFlashCnt(int onMs, int offMs, int count);
void vAnimationPulse(int onMs, int offMs);
void vAnimationPulseCnt(int onMs, int offMs, int count);
void vAnimationRandom(int change_ms);


#endif /* MAIN_DRIVER_ANIMATION_H_ */
