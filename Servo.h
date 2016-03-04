/*
 * Servo.h
 *
 *  Created on: Nov 12, 2015
 *      Author: ulab
 */

#ifndef SOURCES_SERVO_H_
#define SOURCES_SERVO_H_


/*
 * Servo.c
 *
 *  Created on: Nov 12, 2015
 *      Author: ulab
 */

#include "Servo.h"

#include "MK64F12.h"

#define SERVO_FTM_NUM	0
#define SERVO_FTM_CHANNEL 2
#define SERVO_MOD_VALUE   37499
#define SERVO_PRESCALE	5
#define SERVO_NEUTRAL 7.5
#define SERVO_NEUTRAL_ANGLE 0

#define SERVO_PORT C
#define SERVO_PIN 3
#define SERVO_MUX 4


//float y;

void Servo_Initialize();

void Servo_SetAngle(float angle);



#endif /* SOURCES_SERVO_H_ */
