/*
 * LSC.h
 *
 *  Created on: Nov 12, 2015
 *      Author: ulab
 */

#include "DataTypes.h"

#ifndef SOURCES_LSC_H_
#define SOURCES_LSC_H_

//#define THRESHOLD (3.3 /*volts*/ / 0.03125) // 8-bit A/D
#define ADC_MAX_VAL	255
#define ADC_MAX_V	3.3
#define ADC_SET_V	0.325
#define THRESHOLD (ADC_SET_V/ADC_MAX_V)*ADC_MAX_VAL // 8-bit A/D
#define LSC_MIN_WIDTH 10

#define LSC_FTM_NUM		3
#define LSC_FTM_CLK		6

#define LSC_CLK_PORT	C
#define LSC_CLK_PIN		10
#define LSC_CLK_MUX		3

#define LSC_MOD_VALUE   200 //
#define LSC_DATA_CNT	128

#define SI_PORT 	C
#define SI_PIN 		11
#define SI_PIT 		0
#define SI_TIME		15
#define SI_UNITS	ms

#define LSC_ADC_NUM	1
#define LSC_ADC_CHANNEL	15

LineScanCamera LSC;

void LSC_InitializeADC();

void LSC_InitializGPIO();

void LSC_InitializeSI(void);

void LSC_InitializeCLK();

void LSC_Initialize();

void LSC_DisableInterrupt(void);

void LSC_EnableInterrupt(void);

void LSC_SetPriorities(char SI, char CLK, char ADC);

void LSC_ProcessData(void);

void FTM0_IRQHandler(void);

void ADC0_IRQHandler(void);

void PIT0_IRQHandler(void);



#endif /* SOURCES_LSC_H_ */
