/*
 * MotorControl.h
 *
 *  Created on: Nov 13, 2015
 *      Author: ulab
 */

#ifndef SOURCES_DRIVINGCONTROL_H_
#define SOURCES_DRIVINGCONTROL_H_
#include "DataTypes.h"

#define MOTOR_FTM_NUM		0
#define MOTOR_LEFT_CH		1
#define MOTOR_RIGHT_CH		3

#define MOTOR_SPEED_NORMAL	80
#define MOTOR_SPEED_FAST	100
#define MOTOR_SPEED_SLOW	10

#define MOTOR_LEFT_PORT		C
#define MOTOR_LEFT_PIN		2
#define MOTOR_LEFT_MUX		4

#define MOTOR_RIGHT_PORT	C
#define	MOTOR_RIGHT_PIN		4
#define MOTOR_RIGHT_MUX		4

#define MOTOR_RIGHT_REVERSE_PORT	D
#define	MOTOR_RIGHT_REVERSE_PIN		3

#define MOTOR_LEFT_REVERSE_PORT		D
#define MOTOR_LEFT_REVERSE_PIN		1

#define	MOTOR_BRAKE_PORT	B
#define MOTOR_BRAKE_PIN		23
//#define MOTOR_SPEED_FAST	MOTOR_SPEED_NORMAL+30
//#define MOTOR_SPEED_SLOW	MOTOR_SPEED_NORMAL-30

#define MOTOR_MOD_VALUE   37499
#define MOTOR_PRESCALE	5

#define MOTOR_Proportional		15		//40 for speed 40 |60
#define MOTOR_Integral			15		//0 for speed 40 |8
#define MOTOR_Derivative		60	//0 for speed 40 |5
#define MOTOR_KP MOTOR_Proportional/SampleTime
#define MOTOR_KI MOTOR_Integral/SampleTime
#define MOTOR_KD MOTOR_Derivative/SampleTime

#define ACCURACY_MODE_SPEED 	50
#define SPEED_MODE_SPEED		100

DrivingControlData DrivingData;

float MC_GetCV(int percent);

void DrivingControl_Initialize();

void DrivingControl_SetDirection(Direction direction);

void DrivingControl_SetSpeed(Direction, float);

void DrivingControl_SetNormalSpeed(float);

void DrivingControl_HardTurn(Direction direction);

void DrivingControl_AllStop();

void DrivingControl_Go();

void DrivingControl_Intersection();

void DrivingControl_TimerInit(void);

void PIT1_IRQHandler(void);

void DrivingControl_FollowLine();
void DrivingControl_FindLine();
void DrivingControl_BlinkerInit();
void DrivingControl_UpdateBlinker();


#endif /* SOURCES_DRIVINGCONTROL_H_ */
