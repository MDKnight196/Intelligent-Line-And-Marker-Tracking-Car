/*
 * DataTypes.h
 *
 *  Created on: Nov 12, 2015
 *      Author: Zack Rauen
 */
#include <stdbool.h>
#include "MK64F12.h"
#ifndef SOURCES_DATATYPES_H_
#define SOURCES_DATATYPES_H_

//typedef enum { false, true } bool;
typedef enum {LEFT, RIGHT, FORWARD, BACKWARD, NONE} Direction;
typedef enum {A=0, B, C, D, E} Port;
typedef enum {STANDBY, ACCURACY, SPEED, DISCOVERY} State;
typedef enum {s = 1, ms = 1000, us = 1000000, ns = 1000000000} TimeUnits;
typedef enum {RED, GREEN, BLUE, PURPLE, WHITE} Color;

typedef struct {
	unsigned char fallingCount;
	unsigned char data[128];
	bool processedData[128];
	unsigned char location;
	unsigned char lineLength;
	bool hasNewData;
	bool lost;
	bool intersection;
	bool startStopLine;
	bool intersectionPossible;
} LineScanCamera;

typedef struct {
	float lastInput;
	float ITerm;
	float output;
	float rawOutput;
	float setpoint;
	float offset;
	float outmax;
	float outmin;
	float KP;
	float KI;
	float KD;
	bool reverse;
	Direction overflow;
} PIDControl;

typedef struct {
	bool FTMInitialized[4];
	FTM_MemMapPtr FTM[4];
	PORT_MemMapPtr Port[5];
	GPIO_MemMapPtr GPIO[5];
	bool PITInitialized[4];
	PIT_MemMapPtr PITModule;
	bool ADCInitialized[2];
	bool ADCCalibrated[2];
	ADC_MemMapPtr ADC[2];
} CommonData;

typedef struct
{
  uint16_t checksum;
  uint16_t signature;
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  uint16_t angle; // angle is only available for color coded blocks
} Block;

typedef struct
{
  uint8_t normalSpeed;
  Direction turningDirection;
  bool breakreq;
  uint8_t numOfLaps;
  uint8_t lapGoal;
  bool done;
} DrivingControlData;



#endif /* SOURCES_DATATYPES_H_ */
