/*
 * Servo.c
 *
 *  Created on: Nov 12, 2015
 *      Author: Zack Rauen
 */

#include "Servo.h"
#include "Common.h"

void Servo_Initialize() {
	if (Common.FTMInitialized[SERVO_FTM_NUM] == false) {
		Common_InitializeFTM(SERVO_FTM_NUM,SERVO_MOD_VALUE,SERVO_PRESCALE);
	}
	Common_ActivateFTMChannel(SERVO_FTM_NUM, SERVO_FTM_CHANNEL, false, SERVO_NEUTRAL);
	Common_ActivatePin(SERVO_PORT,SERVO_PIN,SERVO_MUX);
}

void Servo_SetAngle(float angle) {
	Common_SetFTMDutyCycle(SERVO_FTM_NUM, SERVO_FTM_CHANNEL, ((0.01*angle + 1.5)/20)*100);
}





