#include "DataTypes.h"

#define STEERING_SETPOINT 64
#define STEERING_OUTMAX 90 // right turn
#define STEERING_OUTMIN -90 // left turn

#define MOTOR_PID_SETPOINT 50
#define MOTOR_PID_OUTMAX 100 // right turn
#define MOTOR_PID_OUTMIN 10 // left turn

#define SampleTime 		15
#define Proportional	45		//40 for speed 40 |60
#define Integral		10		//0 for speed 40 |8
#define Derivative		0		//0 for speed 40 |5
#define STEERING_KP Proportional/SampleTime
#define STEERING_KI Integral/SampleTime
#define STEERING_KD Derivative/SampleTime

PIDControl SteeringPID;
PIDControl LeftPID;
PIDControl RightPID;

void PID_Initialize(PIDControl *, float, float, float, float, bool);
void PID_SetPoint(PIDControl *, float);
void PID_Coefficients(PIDControl *, float, float, float);
//void PID_Compute(PIDControl *, float);
void PID_Compute(PIDControl *PID, float);
void PID_Boundaries(PIDControl *, float, float);
