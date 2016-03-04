/*
 * PID.c
 *
 *  Created on: Nov 12, 2015
 *      Author: Zack Rauen
 */

#include "PID.h"


void PID_Initialize(PIDControl *PID, float setpoint, float outmin, float outmax, float outputOffset, bool reverseError) {
	PID->ITerm = 0;
	PID->lastInput = 65;
	PID->setpoint = setpoint;
	PID->outmin = outmin;
	PID->outmax = outmax;
	PID->offset = outputOffset;
	PID->reverse = reverseError;
}

void PID_SetPoint(PIDControl *PID, float setpoint) {
	PID->setpoint = setpoint;
}

void PID_Coefficients(PIDControl *PID, float KP, float KI, float KD) {
	PID->KP = KP;
	PID->KI = KI;
	PID->KD = KD;
}

void PID_Boundaries(PIDControl *PID, float outmin, float outmax) {
	PID->outmin = outmin;
	PID->outmax = outmax;
}

void PID_Compute(PIDControl *PID, float input) {
	/*Compute all the working error variables*/
	float error = PID->reverse ? PID->setpoint - input : input - PID->setpoint;
	PID->ITerm+= (PID->KI * error);
	if(PID->ITerm > PID->outmax) PID->ITerm=PID->outmax;
	else if(PID->ITerm < PID->outmin) PID->ITerm= PID->outmin;
	float dInput = (input - PID->lastInput);

	/*Compute PID Output*/
	PID->rawOutput = PID->KP * error + PID->ITerm - PID->KD * dInput;

	if(PID->rawOutput > PID->outmax) {
		PID->output = PID->outmax;
		PID->overflow = RIGHT;
	}
	else if(PID->rawOutput < PID->outmin) {
		PID->output = PID->outmin;
		PID->overflow = LEFT;
	}
	else {
		PID->overflow = NONE;
		PID->output = PID->rawOutput;
	}
//	PID.overflow = NONE;
//	PID.output = output;
	PID->output = (PID->offset) + PID->output;

	/*Remember some variables for next time*/
	PID->lastInput = input;
}
