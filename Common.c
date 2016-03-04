/*
 * Common.c
 *
 *  Created on: Nov 13, 2015
 *      Author: Zack Rauen
 */
#include "DataTypes.h"
#include <stdbool.h>
#include "Common.h"
#include "MK64F12.h"

// TO ADD:
// ADC Customization
// ADC Calibration

void Common_Initialize() {
	for (int i=0;i<=3;i++) {
		Common.FTMInitialized[i] = false;
		Common.PITInitialized[i] = false;
		Common.ADCInitialized[i%2] = false;
		Common.ADCCalibrated[i%2] = false;
	}
	Common.FTM[0] = FTM0_BASE_PTR;
	Common.FTM[1] = FTM1_BASE_PTR;
	Common.FTM[2] = FTM2_BASE_PTR;
	Common.FTM[3] = FTM3_BASE_PTR;

	Common.Port[0] = PORTA_BASE_PTR;
	Common.Port[1] = PORTB_BASE_PTR;
	Common.Port[2] = PORTC_BASE_PTR;
	Common.Port[3] = PORTD_BASE_PTR;
	Common.Port[4] = PORTE_BASE_PTR;

	Common.GPIO[0] = PTA_BASE_PTR;
	Common.GPIO[1] = PTB_BASE_PTR;
	Common.GPIO[2] = PTC_BASE_PTR;
	Common.GPIO[3] = PTD_BASE_PTR;
	Common.GPIO[4] = PTE_BASE_PTR;

	Common.PITModule = PIT_BASE_PTR;

	Common.ADC[0] = ADC0_BASE_PTR;
	Common.ADC[1] = ADC1_BASE_PTR;
}

unsigned char Common_BitReverse(unsigned char byte) {
   byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
   byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
   byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
   return byte;
}

void Common_ActivatePin(Port port, uint8_t pin, uint8_t mux) {
	Common.Port[port]->PCR[pin] |= PORT_PCR_MUX(mux) | PORT_PCR_DSE_MASK; // Set as GPIO
}

void Common_ActivatePinGPIO(Port port, uint8_t pin, bool output) {
	Common.Port[port]->PCR[pin] |= PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; // Set as GPIO
	if (output == true) {
		Common_SetBit(Common.GPIO[port]->PDDR, pin);
	}
	else {
		Common_ClearBit(Common.GPIO[port]->PDDR, pin);
	}
}

void Common_GPIOSet(Port port, uint8_t pin) {
	Common_SetBit(Common.GPIO[port]->PSOR,pin);
}

void Common_GPIOClear(Port port, uint8_t pin) {
	Common_SetBit(Common.GPIO[port]->PCOR,pin);
}

void Common_InitializeFTM(unsigned char FTMNumber, int modValue, unsigned char prescale) {
	switch(FTMNumber) {
	case 0:
		SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK; //Enable the clock to the FTM0 module
		break;
	case 1:
		SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK; //Enable the clock to the FTM0 module
		break;
	case 2:
		SIM_SCGC3 |= SIM_SCGC3_FTM2_MASK; //Enable the clock to the FTM0 module
		break;
	case 3:
		SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK; //Enable the clock to the FTM0 module
		break;
	}

	Common.FTM[FTMNumber]->SC = 0;
	Common.FTM[FTMNumber]->CNTIN = 0;
	Common.FTM[FTMNumber]->CNT = 0;
	Common.FTM[FTMNumber]->MOD = modValue;
	Common.FTM[FTMNumber]->CONF = FTM_CONF_BDMMODE(3);
	Common.FTM[FTMNumber]->SC |= FTM_SC_PS(prescale);
	Common.FTM[FTMNumber]->SC |= FTM_SC_CLKS(1);

	Common.FTMInitialized[FTMNumber] = true;
}

void Common_ActivateFTMChannel(unsigned char FTMNumber, unsigned char Channel, bool interrupt, float percent){
	if (interrupt == true) {
		Common.FTM[FTMNumber]->CONTROLS[Channel].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK | FTM_CnSC_CHIE_MASK;
	}
	else {
		Common.FTM[FTMNumber]->CONTROLS[Channel].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
	}
	Common.FTM[FTMNumber]->CONTROLS[Channel].CnV = Common.FTM[FTMNumber]->MOD * (percent/100);
}

void Common_SetFTMDutyCycle(unsigned char FTMNumber, unsigned char Channel,  float percent) {
	Common.FTM[FTMNumber]->CONTROLS[Channel].CnV = Common.FTM[FTMNumber]->MOD * (percent/100);
}

void Common_ClearFTMFlag(unsigned char FTMNumber, unsigned char Channel) {
	int read;
	read = Common.FTM[FTMNumber]->CONTROLS[Channel].CnSC;
	Common.FTM[FTMNumber]->CONTROLS[Channel].CnSC &= ~FTM_CnSC_CHF_MASK;
}

void Common_SetupPIT(uint8_t channel, unsigned int time, TimeUnits units) {
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	Common.PITModule->MCR = 0;
	Common.PITModule->CHANNEL[channel].LDVAL = (60000000/(units/time))-1;	// 1/20Mhz = 50ns   (.005s/50ns)-1= 99,999 cycles or 0x1869F
	Common.PITModule->CHANNEL[channel].TCTRL |= PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK; // Enable interrupt and enable timer
	Common.PITInitialized[channel] = true;
}

void Common_SetPITTime(uint8_t channel, unsigned int time, TimeUnits units) {
	Common.PITModule->CHANNEL[channel].LDVAL = (60000000/(units/time))-1;
}

void Common_ClearPITFlag(uint8_t channel) {
	Common.PITModule->CHANNEL[channel].TFLG = PIT_TFLG_TIF_MASK;
}

// 8 bit high speed mode, no averaging
void Common_InitializeADC(uint8_t adcNumber) {
	if (adcNumber == 1)
		SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK; /*Enable the ADC0 Clock*/
	if (adcNumber == 0)
		SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
	Common.ADC[adcNumber]->CFG1 |= ADC_CFG1_MODE(0);
	Common.ADC[adcNumber]->SC1[0] |= ADC_SC1_ADCH(31) | ADC_SC1_AIEN_MASK;//00101 to read
	Common.ADC[adcNumber]->CFG2 |= ADC_CFG2_MUXSEL(1) | ADC_CFG2_ADHSC_MASK;
	Common.ADCInitialized[adcNumber] = true;
}

uint8_t Common_ReadADCChannel(uint8_t adcNumber, uint8_t channel) {
	Common.ADC[adcNumber]->SC1[0] = ADC_SC1_ADCH(channel); //Write to SC1A to start conversion
	while(Common.ADC[adcNumber]->SC2 & ADC_SC2_ADACT_MASK); //Conversion in progress
	while(!(Common.ADC[adcNumber]->SC1[0] & ADC_SC1_COCO_MASK)); //Wait until conversion complete
	return Common.ADC[adcNumber]->R[0];
}

void Common_StartADCRead(uint8_t adcNumber, uint8_t channel) {
	Common.ADC[adcNumber]->SC1[0] = ADC_SC1_ADCH(channel) | ADC_SC1_AIEN_MASK;
}

// 8 bit high speed mode, no averaging
uint8_t Common_GetADCValue(uint8_t adcNumber) {
	return Common.ADC[adcNumber]->R[0];
}

Direction Common_oppositeDirection(Direction in) {
	switch(in) {
	case LEFT:
		return RIGHT;
	case RIGHT:
		return LEFT;
	case FORWARD:
		return BACKWARD;
	default: return NONE;
	}
}

