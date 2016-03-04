/*
 * LCD.h
 *
 *  Created on: Dec 1, 2015
 *      Author: ulab
 */

#ifndef SOURCES_LCD_H_
#define SOURCES_LCD_H_

#include "Common.h"
#include "DataTypes.h"
#include "fsl_i2c_master_driver.h"


#define LCD_DATA_LENGTH 21
#define PORT_A_ADDR 0x12
#define PORT_A_INTF 0x0E
#define PORT_B_ADDR 0x13

#define LCD_BTN_SELECT 1
#define LCD_BTN_RIGHT 2
#define LCD_BTN_DOWN 4
#define LCD_BTN_UP 8
#define LCD_BTN_LEFT 16

i2c_master_state_t master;
Color LCD_currentColor;

uint8_t cmdCnt;
uint8_t cmdFLG[1];
uint8_t cmdCAP[1];

uint8_t LCD_readBuff[LCD_DATA_LENGTH];

uint32_t count;
bool LCD_modeSelected;
bool LCD_buttonJustPressed;
uint8_t LCD_buttonPressed,LCD_debounceCheck;

void LCD_SendCommand(uint8_t data);

void LCD_SendData(uint8_t data);

void LCD_SendChar(unsigned char letter);

void LCD_SendWord(unsigned char * word);

void LCD_ChangeColor(Color newcolor);
void LCD_Clear();
void LCD_ReadButton();

void LCD_Initialize();


#endif /* SOURCES_LCD_H_ */
