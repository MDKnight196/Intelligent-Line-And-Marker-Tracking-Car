/*
Author: Zack Rauen
*/
#include "MK64F12.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "DataTypes.h"
#include "Common.h"
#include "LSC.h"
#include "PID.h"
#include "PIXY.h"
#include "LCD.h"
#include "DrivingControl.h"
#include "ModeControl.h"
#include "Servo.h"

void debugPrint();
void LCD_ButtonTimerInit(void);
void PIT2_IRQHandler(void);


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/

int main (void) {
	// Initialize basic things
	hardware_init();
	dbg_uart_init();
	BOARD_ClockInit();
	OSA_Init();
	
	// Initialize my code.
	Common_Initialize(); // Initialize core abstraction layer
	I2C_Initialize(); // Initialize I2C0 for LCD and PIXY
	LCD_Initialize(); // Initialize LCD with standard message
	LSC_Initialize(); // Default settings, interrupts enabled, priorities 5 6 7
	DrivingControl_Initialize(); // Initialize steering and motor PWMs
	LCD_ButtonTimerInit(); // Start button reading from LCD panel
	ModeControl_Initialize(); // Begin in STANDBY
  
  while (1) {
	switch (mode) {
		case STANDBY:
			previousMode=mode;
			DrivingControl_AllStop();
			DrivingData.breakreq = false;
			DrivingData.lapGoal = 2;
			break;
		case ACCURACY:
			previousMode=mode;
			DrivingControl_SetNormalSpeed(ACCURACY_MODE_SPEED);
			DrivingData.breakreq = false;
			DrivingControl_FollowLine();
			if (DrivingData.done == true)
				ModeControl_changeMode(STANDBY);
			break;
		case SPEED:
			previousMode=mode;
			DrivingData.lapGoal = 2;
			DrivingControl_SetNormalSpeed(SPEED_MODE_SPEED);
			DrivingData.breakreq = false;
			DrivingControl_FollowLine();
			if (DrivingData.done == true)
				ModeControl_changeMode(STANDBY);
			break;
		case DISCOVERY:
			previousMode=mode;
			DrivingData.breakreq = false;
			DrivingData.lapGoal = 1;
			DrivingControl_FindLine();
			if (DrivingData.breakreq == false)
				ModeControl_changeMode(ACCURACY);
			break;
		default: break;
	}

//	  debugPrint(); // Use for debugging
  }
}

void LCD_ButtonTimerInit(void) {
	Common_SetupPIT(2, 50, ms);
	NVIC_EnableIRQ(PIT2_IRQn);
	NVIC_SetPriority(PIT2_IRQn,5);
}

void PIT2_IRQHandler(void) {
	Common_ClearPITFlag(2); // Clear interrupt
	LCD_ReadButton();
//	printf("Button: %d\r\n", LCD_buttonPressed);
    switch (LCD_buttonPressed) {
    case LCD_BTN_SELECT:
    	LCD_modeSelected = true;
    	break;
    case LCD_BTN_RIGHT:
    	if (nextMode == NUM_OF_MODES-1) {
    		nextMode = 0;
    	}
    	else {
    		nextMode++;
    	}
    	LCD_buttonJustPressed = true;
    	DrivingData.breakreq = true;
    	break;
    case LCD_BTN_LEFT:
    	if (nextMode == 0) {
    		nextMode = NUM_OF_MODES-1;
    	}
    	else {
    		nextMode--;
    	}
    	LCD_buttonJustPressed = true;
    	DrivingData.breakreq = true;
    	break;
    default: break;
    }
}

void debugPrint() {
	char *dirString;
	switch (DrivingData.turningDirection) {
	case 0:
		dirString = "LEFT";
		break;
	case 1:
		dirString = "RIGHT";
		break;
	case 2:
		dirString = "FORWARD";
		break;
	case 3:
		dirString = "BACKWARD";
		break;
	case 4:
		dirString = "NONE";
		break;
	}
	printf("\r\n\r\n");
	for (int i = 0; i <= 127; i++) { // ignore the first and last 16 bits in the camera frame
		printf("%d",LSC.processedData[i] ? 1 : 0);
	}
	printf("\r\n\r\n");
	printf("Location: %d", LSC.location);
	printf(" | Line Length: %d", LSC.lineLength);
	printf(" | Intersection Possible?: %s", LSC.intersectionPossible == true ? "YES" : "NO");
	printf(" | Left: %f | Right: %f", LeftPID.output, RightPID.output);
	printf(" | Direction: %s", dirString);
}


