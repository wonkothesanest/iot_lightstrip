/*
 * blink.h
 *
 *  Created on: Jul 23, 2018
 *      Author: dustin
 */

#ifndef MAIN_DRIVERS_PWM_H_
#define MAIN_DRIVERS_PWM_H_

#define PWM_PIN 2

void vPwmStart();
void vPwmSetValue(int v);
int iPwmGetValue();

#endif /* MAIN_DRIVERS_PWM_H_ */
