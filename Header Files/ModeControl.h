/*
 * ModeControl.h
 *
 *  Created on: Nov 24, 2015
 *      Author: Zack Rauen
 */

#include "DataTypes.h"

#ifndef SOURCES_MODECONTROL_H_
#define SOURCES_MODECONTROL_H_

#define NUM_OF_MODES 4
#define NUM_LEDS  4

int32_t LEDcount;
State mode;
State previousMode;

void ModeControl_Initialize(void);
void ModeControl_changeMode(int new);
void ModeControl_nextMode();
void PORTC_IRQHandler(void);
void LED_Initialize(void);
void LED_On (unsigned int idx);
void LED_Off (unsigned int idx);




#endif /* SOURCES_MODECONTROL_H_ */
