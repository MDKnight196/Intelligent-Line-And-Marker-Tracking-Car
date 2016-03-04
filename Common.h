/*
 * Common.h
 *
 *  Created on: Nov 13, 2015
 *      Author: ulab
 */

#ifndef SOURCES_COMMON_H_
#define SOURCES_COMMON_H_

#include "DataTypes.h"
#include "fsl_i2c_master_driver.h"

#define Shift(x,n) (x << n);
#define Common_SetBit(port, pin)		BITBAND_REG(port,pin) = 1;
#define Common_ClearBit(port, pin)		BITBAND_REG(port,pin) = 0;

CommonData Common;

i2c_master_state_t I2C_Master;

void Common_Initialize();

unsigned char Common_BitReverse(unsigned char);

void Common_ActivatePin(Port port, uint8_t pin, uint8_t mux);
void Common_ActivatePinGPIO(Port port, uint8_t pin, bool input);
void Common_GPIOSet(Port port, uint8_t pin);
void Common_GPIOClear(Port port, uint8_t pin);

void Common_InitializeFTM(unsigned char FTMNumber, int modValue, unsigned char prescale);
void Common_ActivateFTMChannel(unsigned char FTMNumber, unsigned char Channel, bool interrupt, float percent);
void Common_SetFTMDutyCycle(unsigned char FTMNumber, unsigned char Channel,  float percent);
void Common_ClearFTMFlag(unsigned char FTMNumber, unsigned char Channel);

void Common_SetupPIT(uint8_t channel, unsigned int time, TimeUnits units);
void Common_SetPITTime(uint8_t channel, unsigned int time, TimeUnits units);
void Common_ClearPITFlag(uint8_t channel);

void Common_InitializeADC(uint8_t adcNumber);
uint8_t Common_ReadADCChannel(uint8_t adcNumber, uint8_t channel);
void Common_StartADCRead(uint8_t adcNumber, uint8_t channel);
uint8_t Common_GetADCValue(uint8_t adcNumber);

Direction Common_oppositeDirection(Direction in);





#endif /* SOURCES_COMMON_H_ */
