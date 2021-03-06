/*
 * blink.h
 *
 *  Created on: Jul 23, 2018
 *      Author: dustin
 */

#ifndef MAIN_DRIVERS_PWM_H_
#define MAIN_DRIVERS_PWM_H_
#include "HSVToRGB.h"

#define PWM_PIN 2

/**
 * Initialization of the PWM state of the pins and driver setup
 */
void vPwmStart();

/**
 * Sets an HSV value to the pwm pins
 */
void vPwmSetValue(struct HSV hsv);
void vPwmSetValueRec(struct HSV hsv, bool record, bool fade);

/**
 * Gets the HSV value of the current system.
 */
struct HSV iPwmGetValue();
/**
 * Turns off the lights
 */
void vPWMTurnOff();
/**
 * Turns on the lights
 */
void vPWMTurnOn();

#endif /* MAIN_DRIVERS_PWM_H_ */
