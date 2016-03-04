/*
 * ModeControl.c
 *
 *  Created on: Nov 24, 2015
 *      Author: Zack Rauen
 */

#include "ModeControl.h"
#include "Common.h"
#include "DrivingControl.h"
#include "LCD.h"

void ModeControl_Initialize(void) {
	LED_Initialize(); // TEMP
	PORTC_PCR5 = PORT_PCR_IRQC(10) | PORT_PCR_MUX(1) ;
	GPIOC_PDDR |= (0 << 5);
	NVIC_EnableIRQ(PORTC_IRQn);// TEMP

	ModeControl_changeMode(0);
	previousMode = NUM_OF_MODES-1;
}

void ModeControl_changeMode(int new) {
	if (new >= NUM_OF_MODES){
		return;
	}
	LED_Off(mode);
	if (new < 0) {
		mode = 0;
	}
	else {
		mode = new;
	}

	LED_On(mode);
//	LCD_ChangeColor(mode);
//	DrivingData.breakreq = true;
}

void ModeControl_nextMode() {
	ModeControl_changeMode(mode >= NUM_OF_MODES-1 ? 0 : mode+1);
}

void PORTC_IRQHandler(void) {
	for (int i=0; i < 1000; i++) {}
	if ((GPIOC_PDIR & 0x00000001) == 0)
		ModeControl_nextMode();
	PORTC_ISFR = PORT_ISFR_ISF(0x20);
}

void LED_Initialize(void) {
  Common_ActivatePinGPIO(B,22,true);
  Common_ActivatePinGPIO(B,21,true);
  Common_ActivatePinGPIO(E,26,true);
  Common_GPIOSet(B,22);
  Common_GPIOSet(B,21);
  Common_GPIOSet(E,26);
}

/*----------------------------------------------------------------------------
  Function that turns on requested LED
 *----------------------------------------------------------------------------*/
void LED_On (unsigned int idx) {
  switch (idx) {
  case 0:
	  Common_GPIOClear(B,22);
	  break;
  case 1:
	  Common_GPIOClear(B,21);
	  break;
  case 2:
	  Common_GPIOClear(E,26);
	  break;
  case 3:
	  Common_GPIOClear(B,22);
	  Common_GPIOClear(B,21);
	  break;
  default: break;
  }
}

/*----------------------------------------------------------------------------
  Function that turns off requested LED
 *----------------------------------------------------------------------------*/
void LED_Off (unsigned int idx) {
  switch (idx) {
  case 0:
	  Common_GPIOSet(B,22);
	  break;
  case 1:
	  Common_GPIOSet(B,21);
	  break;
  case 2:
	  Common_GPIOSet(E,26);
	  break;
  case 3:
	  Common_GPIOSet(B,22);
	  Common_GPIOSet(B,21);
	  break;
  default: break;
  }
}
