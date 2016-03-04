/*
Author: Zack Rauen
*/

#include "LSC.h"
#include "Common.h"

#include "PIXY.h"
#include "DrivingControl.h"

void LSC_InitializeADC() {
	Common_InitializeADC(LSC_ADC_NUM);
}

void LSC_InitializGPIO() {
	Common_ActivatePinGPIO(SI_PORT,SI_PIN,true);
	Common_ActivatePin(LSC_CLK_PORT,LSC_CLK_PIN,LSC_CLK_MUX);
}

void LSC_InitializeSI(void) {
	Common_SetupPIT(SI_PIT,SI_TIME,SI_UNITS);
}

void LSC_InitializeCLK() {
	if (Common.FTMInitialized[LSC_FTM_NUM] == false) {
		Common_InitializeFTM(LSC_FTM_NUM, LSC_MOD_VALUE, 0);
	}
	Common_ActivateFTMChannel(LSC_FTM_NUM, LSC_FTM_CLK, true, 0);
}

void LSC_DisableInterrupt(void) {
	NVIC_DisableIRQ(FTM3_IRQn);
	NVIC_DisableIRQ(PIT0_IRQn);
	NVIC_DisableIRQ(ADC1_IRQn);
}

void LSC_EnableInterrupt(void) {
	NVIC_EnableIRQ(ADC1_IRQn);
	NVIC_EnableIRQ(FTM3_IRQn);
	NVIC_EnableIRQ(PIT0_IRQn);
}

void LSC_SetPriorities(char SI, char CLK, char ADC) {
	NVIC_SetPriority(PIT0_IRQn, SI);
	NVIC_SetPriority(FTM3_IRQn, CLK);
	NVIC_SetPriority(ADC1_IRQn, ADC);
}

void LSC_Initialize() {
	LSC.fallingCount = 0;
	LSC.lineLength = 0;
	LSC.location = 65;
	LSC.hasNewData = false;
	LSC.lost = false;
	LSC.intersection = false;
	LSC.startStopLine = false;
	LSC.intersectionPossible = false;
	LSC_InitializeADC();
	LSC_InitializGPIO();
	LSC_InitializeSI();
	LSC_InitializeCLK();
	LSC_EnableInterrupt();
//	LSC_SetPriorities(5,6,7);
}

int LSC_FindLocation(int startVal) {
	int location, offset;
	for (int i = startVal; i <= LSC_DATA_CNT-2; i++) {
		offset = 0;
		if (LSC.processedData[i] == true) {
			while (LSC.processedData[i] == LSC.processedData[i+offset]) offset++;
			LSC.lineLength = (offset+i)-i;
			if (LSC.lineLength >= LSC_MIN_WIDTH) {
				location = ((offset+i)+i)/2;
				return location;
			}
		}
	}
	return -1;
}

void LSC_ProcessData(void) {
	LSC.intersection = false;
	LSC.lost = false;
	LSC.startStopLine = false;
	int location, testLocation;
	for (int i = 0; i <= LSC_DATA_CNT-1; i++) { // ignore the first and last bits in the camera frame
	  if (LSC.data[i] < THRESHOLD)
		  LSC.processedData[i] = true; // black (low intensity)
	  else
		  LSC.processedData[i] = false; // white (high intensity)
	}

	location = LSC_FindLocation(0);
	if (location == -1) {
		LSC.lost = true;
	}
	else if (location <= 30) {
		testLocation = LSC_FindLocation(location+10);
		if (testLocation <= 80) {
			LSC.location = testLocation;
			testLocation = LSC_FindLocation(LSC.location+10);
			if (testLocation > 80) {
				LSC.startStopLine = true;
			}
		}
	}
	if (LSC.startStopLine == false && LSC.lost == false) {
		LSC.location = location;
		if (LSC.lineLength >= 125 ) {
			if (LSC.intersectionPossible == true) {
				LSC.intersection = true;
				LSC.intersectionPossible = false;
			}
			else {
				LSC.intersectionPossible = true;
				LSC.intersection = false;
			}
		}
		else {
			LSC.intersection = false;
		}
	}
	LSC.hasNewData = true;
}

void FTM3_IRQHandler(void) {
	Common_ClearFTMFlag(LSC_FTM_NUM,LSC_FTM_CLK);
	Common_StartADCRead(LSC_ADC_NUM,LSC_ADC_CHANNEL);
	LSC.fallingCount++;
	if (LSC.fallingCount>=LSC_DATA_CNT+1) {
		Common_SetFTMDutyCycle(LSC_FTM_NUM,LSC_FTM_CLK, 0);
	}
	Common_GPIOClear(SI_PORT,SI_PIN);
}

void PIT0_IRQHandler(void)
{
	Common_ClearPITFlag(SI_PIT);
	Common_GPIOSet(SI_PORT,SI_PIN);
	Common_SetFTMDutyCycle(LSC_FTM_NUM,LSC_FTM_CLK, 50);
	LSC.fallingCount=0;
}

void ADC1_IRQHandler(void) {
	if (LSC.fallingCount>0)
		LSC.data[LSC.fallingCount-1] = Common_GetADCValue(LSC_ADC_NUM);
	else
		Common_GetADCValue(LSC_ADC_NUM);
	if (LSC.fallingCount>=LSC_DATA_CNT+1) {
		LSC_ProcessData();
	}
}


