/*
 * MotorControl.c
 *
 *  Created on: Nov 13, 2015
 *      Author: Zack Rauen
 */
#include "DrivingControl.h"
#include "DataTypes.h"
#include "Common.h"
#include "LSC.h"
#include "fsl_i2c_master_driver.h"
#include "PIXY.h"
#include "Servo.h"
#include "PID.h"

#define BLINKER_LEFT 18
#define BLINKER_RIGHT 19

void DrivingControl_Initialize() {
	Servo_Initialize();
	Servo_SetAngle(SERVO_NEUTRAL_ANGLE);
	DrivingData.normalSpeed = 0;
	if (Common.FTMInitialized[MOTOR_FTM_NUM] == false) {
		Common_InitializeFTM(MOTOR_FTM_NUM,MOTOR_MOD_VALUE,MOTOR_PRESCALE);
	}
	Common_ActivateFTMChannel(MOTOR_FTM_NUM, MOTOR_LEFT_CH, false, 100-DrivingData.normalSpeed);
	Common_ActivateFTMChannel(MOTOR_FTM_NUM, MOTOR_RIGHT_CH, false, 100-DrivingData.normalSpeed);

	Common_ActivatePinGPIO(MOTOR_BRAKE_PORT,MOTOR_BRAKE_PIN,true);
	Common_ActivatePinGPIO(MOTOR_RIGHT_REVERSE_PORT,MOTOR_RIGHT_REVERSE_PIN,true);
	Common_ActivatePinGPIO(MOTOR_LEFT_REVERSE_PORT,MOTOR_LEFT_REVERSE_PIN,true);
	Common_ActivatePin(MOTOR_RIGHT_PORT,MOTOR_RIGHT_PIN,MOTOR_RIGHT_MUX);
	Common_ActivatePin(MOTOR_LEFT_PORT,MOTOR_LEFT_PIN,MOTOR_LEFT_MUX);

	Common_GPIOSet(MOTOR_RIGHT_REVERSE_PORT,MOTOR_RIGHT_REVERSE_PIN);
	Common_GPIOSet(MOTOR_LEFT_REVERSE_PORT,MOTOR_LEFT_REVERSE_PIN);
	Common_GPIOSet(MOTOR_BRAKE_PORT,MOTOR_BRAKE_PIN);

	DrivingData.turningDirection = LEFT;

	PID_Initialize(&SteeringPID, STEERING_SETPOINT, STEERING_OUTMIN, STEERING_OUTMAX, 0, false);
	PID_Coefficients(&SteeringPID, STEERING_KP, STEERING_KI, STEERING_KD);
	PID_Initialize(&LeftPID, SERVO_NEUTRAL_ANGLE, MOTOR_PID_OUTMIN, MOTOR_PID_OUTMAX, 0, false);
	PID_Coefficients(&LeftPID, MOTOR_KP, MOTOR_KI, MOTOR_KD);
	PID_Initialize(&RightPID, SERVO_NEUTRAL_ANGLE, MOTOR_PID_OUTMIN, MOTOR_PID_OUTMAX, 0, true);
	PID_Coefficients(&RightPID, MOTOR_KP, MOTOR_KI, MOTOR_KD);

	DrivingData.breakreq = false;
	Pixy_Init();
	DrivingControl_TimerInit();
	DrivingControl_BlinkerInit();
	DrivingData.numOfLaps = 0;
	DrivingData.lapGoal = 2;
	DrivingData.done = false;
}

void DrivingControl_SetDirection(Direction direction) {
	Common_GPIOClear(C,1);
	Common_GPIOSet(MOTOR_RIGHT_REVERSE_PORT,MOTOR_RIGHT_REVERSE_PIN);
	Common_GPIOSet(MOTOR_LEFT_REVERSE_PORT,MOTOR_LEFT_REVERSE_PIN);
	if (direction == LEFT) {
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_RIGHT_CH, 100-MOTOR_SPEED_FAST);
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_LEFT_CH, 100-MOTOR_SPEED_SLOW);
	}
	else if (direction == RIGHT) {
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_LEFT_CH, 100-MOTOR_SPEED_FAST);
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_RIGHT_CH, 100-MOTOR_SPEED_SLOW);
	}
	else {
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_LEFT_CH, 100-DrivingData.normalSpeed);
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_RIGHT_CH, 100-DrivingData.normalSpeed);
	}
}

void DrivingControl_HardTurn(Direction direction) {
	Common_GPIOClear(C,1);
	if (direction == LEFT) {
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_RIGHT_CH, 100-MOTOR_SPEED_FAST);
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_LEFT_CH, 100-MOTOR_SPEED_FAST); // REVERSE ME
		Common_GPIOClear(MOTOR_LEFT_REVERSE_PORT,MOTOR_LEFT_REVERSE_PIN);
	}
	else if (direction == RIGHT || direction == BACKWARD) {
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_LEFT_CH, 100-MOTOR_SPEED_FAST);
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_RIGHT_CH, 100-MOTOR_SPEED_FAST); // REVERSE ME
		Common_GPIOClear(MOTOR_RIGHT_REVERSE_PORT,MOTOR_RIGHT_REVERSE_PIN);
	}
	else {
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_LEFT_CH, 100-DrivingData.normalSpeed);
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_RIGHT_CH, 100-DrivingData.normalSpeed);
	}
}

void DrivingControl_SetSpeed(Direction direction, float speed) {
	if (direction == LEFT) {
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_LEFT_CH, 100-speed);
	}
	else if (direction == RIGHT) {
		Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_RIGHT_CH, 100-speed);
	}
}

void DrivingControl_SetNormalSpeed(float speed) {
	DrivingData.normalSpeed = speed;
	DrivingControl_SetDirection(FORWARD);
}

void DrivingControl_Intersection() {
	if (DrivingData.turningDirection == RIGHT || DrivingData.turningDirection == LEFT || DrivingData.turningDirection == BACKWARD) {
		DrivingControl_AllStop();
		for (int i=0;i<1000000;i++){}
	}

	int overshoot = -84000 * DrivingData.normalSpeed + 10200000; // Overshoot occurs based on how fast the car is moving

	if (DrivingData.turningDirection == RIGHT || DrivingData.turningDirection == LEFT) {
		DrivingControl_SetDirection(FORWARD);
		for (int i=0;i<overshoot;i++){}
	}
	if (DrivingData.turningDirection == RIGHT || DrivingData.turningDirection == LEFT || DrivingData.turningDirection == BACKWARD) {
		DrivingControl_HardTurn(DrivingData.turningDirection);
		while (LSC.lost == false){}
	}
	if (DrivingData.turningDirection == BACKWARD) {
		while (LSC.lost == true){}
		while (LSC.lost == false){}
		while (LSC.lost == true){}
	}
	if (DrivingData.turningDirection == RIGHT || DrivingData.turningDirection == LEFT || DrivingData.turningDirection == BACKWARD) {
		while ((LSC.location < 55 || LSC.location > 75)){}
	}
	DrivingControl_SetDirection(FORWARD);
}

void DrivingControl_AllStop() {
	Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_LEFT_CH, 100);
	Common_SetFTMDutyCycle(MOTOR_FTM_NUM, MOTOR_RIGHT_CH, 100);
	Common_GPIOSet(C,1);
}

void DrivingControl_Go() {
	DrivingControl_SetDirection(FORWARD);
}

void DrivingControl_TimerInit(void) {
	Common_SetupPIT(1, 20, ms);
	NVIC_EnableIRQ(PIT1_IRQn);
	NVIC_SetPriority(PIT1_IRQn,1);
}

void PIT1_IRQHandler(void)
{
	Common_ClearPITFlag(1);
	Direction grab = Pixy_GetDirection();
	if (grab != NONE)
		DrivingData.turningDirection = grab;
	DrivingControl_UpdateBlinker();
}

void DrivingControl_FollowLine() {
	if (LSC.hasNewData == true) {
		if (LSC.startStopLine == true) {
			DrivingData.numOfLaps++;
			if (DrivingData.numOfLaps == DrivingData.lapGoal) {
				DrivingData.done = true;
				return;
			}
			else {
				while (LSC.startStopLine == true) {} // wait until it fully passes the line.
			}
		}
		else if (LSC.intersection == true) {
			Servo_SetAngle(SERVO_NEUTRAL_ANGLE);
			DrivingControl_Intersection();
			PID_Compute(&SteeringPID, LSC.location);
			Servo_SetAngle(SteeringPID.output);
			SteeringPID.overflow = NONE;
		}
		else if (LSC.lost == false) {
			PID_Compute(&SteeringPID, LSC.location);
			Servo_SetAngle(SteeringPID.output);
			if (SteeringPID.overflow != NONE) {
				PID_Compute(&LeftPID, SteeringPID.output);
				PID_Compute(&RightPID, SteeringPID.output);
				DrivingControl_SetSpeed(LEFT, LeftPID.output);
				DrivingControl_SetSpeed(RIGHT, RightPID.output);
			}
			else {
				DrivingControl_SetDirection(FORWARD);
			}
		}
		else { // This means the car is 'lost' by seeing all white pixels
			if (SteeringPID.overflow == RIGHT || SteeringPID.overflow == LEFT) {
				DrivingControl_HardTurn(SteeringPID.overflow); // Turn very hard to the direction the line was last seen
				while (LSC.lost == false){} // keep turning until it sees the black line
				while ((LSC.location < 55 || LSC.location > 75) && DrivingData.breakreq == false){} // try and center about the line before following again
			}
		}
		LSC.hasNewData = false;
	  }
}

#define discover_timeout 41666666
void DrivingControl_FindLine() {
	int delay = 0;
	Servo_SetAngle(-90);
	DrivingControl_SetDirection(FORWARD);
	DrivingControl_SetSpeed(LEFT, 40);
	DrivingControl_SetSpeed(RIGHT, 80);
	while ((LSC.location < 55 || LSC.location > 75) && DrivingData.breakreq == false){
		delay++;
		if (delay == discover_timeout)
			break;
	}
	if (delay != discover_timeout)
		return;

	Servo_SetAngle(90);
	DrivingControl_SetDirection(FORWARD);
	DrivingControl_SetSpeed(LEFT, 80);
	DrivingControl_SetSpeed(RIGHT, 40);
	while ((LSC.location < 55 || LSC.location > 75) && DrivingData.breakreq == false);
}

void DrivingControl_BlinkerInit() {
	Common_ActivatePinGPIO(B,BLINKER_LEFT,true);
	Common_ActivatePinGPIO(B,BLINKER_RIGHT,true);
	Common_GPIOSet(B,BLINKER_RIGHT);
	Common_GPIOSet(B,BLINKER_LEFT);
	Common_ActivatePinGPIO(C,1,true);
	Common_GPIOClear(C,1);
}

void DrivingControl_UpdateBlinker() {
	switch (DrivingData.turningDirection) {
	case LEFT:
		Common_GPIOClear(B,BLINKER_RIGHT);
		Common_GPIOSet(B,BLINKER_LEFT);
		break;
	case RIGHT:
		Common_GPIOSet(B,BLINKER_RIGHT);
		Common_GPIOClear(B,BLINKER_LEFT);
		break;
	case FORWARD:
		Common_GPIOSet(B,BLINKER_RIGHT);
		Common_GPIOSet(B,BLINKER_LEFT);
		break;
	case BACKWARD:
		Common_GPIOClear(B,BLINKER_RIGHT);
		Common_GPIOClear(B,BLINKER_LEFT);
		break;
	default: break;
	}
}
