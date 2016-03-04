/*
Author: Zack Rauen
*/

#include "LCD.h"
#include "fsl_i2c_master_driver.h"
#include "board.h"

i2c_device_t deviceLCD =
{
  .address = 0x20,
  .baudRate_kbps = 400   // 400 Kbps
};

void LCD_SendCommand(uint8_t data) {
	uint8_t count =  2;
	uint8_t tx[2];
	uint8_t foobar = (LCD_currentColor == BLUE || LCD_currentColor == PURPLE || LCD_currentColor == WHITE) ? 0 : 1;
	tx[0] = PORT_B_ADDR;
	tx[1] = Common_BitReverse(data) >> 4;
	tx[1] = tx[1] << 1;
	tx[1] = (tx[1] | foobar) & ~0x80;
	I2C_DRV_MasterSendDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, NULL, 0, (const uint8_t*)tx, count, 5);
	tx[1] = tx[1] | 0x20;
	OSA_TimeDelay(1);
	I2C_DRV_MasterSendDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, NULL, 0, (const uint8_t*)tx, count, 5);
	tx[1] = tx[1] & ~0x20;
	OSA_TimeDelay(1);
	I2C_DRV_MasterSendDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, NULL, 0, (const uint8_t*)tx, count, 5);
}

void LCD_SendData(uint8_t data) {
	uint8_t count =  2;
	uint8_t tx[2];
	uint8_t foobar = (LCD_currentColor == BLUE || LCD_currentColor == PURPLE || LCD_currentColor == WHITE) ? 0 : 1;
	tx[0] = PORT_B_ADDR;
	tx[1] = Common_BitReverse(data) >> 4;
	tx[1] = tx[1] << 1;
	tx[1] = tx[1] | foobar  | 0x80;
	I2C_DRV_MasterSendDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, NULL, 0, (const uint8_t*)tx, count, 5);
	tx[1] = tx[1] | 0x20;
	OSA_TimeDelay(1);
	I2C_DRV_MasterSendDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, NULL, 0, (const uint8_t*)tx, count, 5);
	tx[1] = tx[1] & ~0x20;
	OSA_TimeDelay(1);
	I2C_DRV_MasterSendDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, NULL, 0, (const uint8_t*)tx, count, 5);
}

void LCD_SendChar(unsigned char letter) {
    LCD_SendData((letter & 0xF0) >> 4);
    LCD_SendData(letter & 0x0F);
}

void LCD_SendWord(unsigned char * word) {
	for(int i = 0; word[i] != '\0'; i++) {
		LCD_SendChar(word[i]);
	}
}

void LCD_ChangeColor(Color newcolor) {
	uint8_t count =  2;
	uint8_t tx[3];
	LCD_currentColor=newcolor;
	tx[0] = PORT_A_ADDR;
	tx[1] = 0xC0;
	tx[2] = 1;
	I2C_DRV_MasterSendDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, NULL, 0, (const uint8_t*)tx, 3, 5);


	switch (LCD_currentColor) {
	case RED:
		tx[0] = PORT_A_ADDR;
		tx[1] = 0x80;
		break;
	case BLUE:
		tx[0] = PORT_B_ADDR;
		tx[1] = 0x0;
		break;
	case GREEN:
		tx[0] = PORT_A_ADDR;
		tx[1] = 0x40;
		break;
	case PURPLE:
		tx[0] = PORT_A_ADDR;
		tx[1] = 0x80;
		I2C_DRV_MasterSendDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, NULL, 0, (const uint8_t*)tx, count, 5);
		tx[0] = PORT_B_ADDR;
		tx[1] = 0;
		break;
	case WHITE:
		tx[0] = PORT_A_ADDR;
		tx[1] = 0x00;
		I2C_DRV_MasterSendDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, NULL, 0, (const uint8_t*)tx, count, 5);
		tx[0] = PORT_B_ADDR;
		tx[1] = 0;
		break;
	default: break;
	}
	I2C_DRV_MasterSendDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, NULL, 0, (const uint8_t*)tx, count, 5);
}

void LCD_Clear() {
	LCD_SendCommand(0x0);		//0
	LCD_SendCommand(0x1);		//0
}

void LCD_ReadButton() {
    for (int i=0; i<LCD_DATA_LENGTH;i++) {
    	LCD_readBuff[i] = 0;
    }
    I2C_DRV_MasterReceiveDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, (const uint8_t*)cmdCAP, cmdCnt, LCD_readBuff, cmdCnt, 5);
    LCD_buttonPressed = LCD_readBuff[0];
    LCD_debounceCheck = LCD_readBuff[0];
    I2C_DRV_MasterReceiveDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, (const uint8_t*)cmdFLG, cmdCnt, LCD_readBuff, cmdCnt, 5);

    while (LCD_buttonPressed == LCD_debounceCheck && LCD_buttonPressed != 0) {
    	I2C_DRV_MasterReceiveDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, (const uint8_t*)cmdCAP, cmdCnt, LCD_readBuff, cmdCnt, 5);
    	LCD_debounceCheck = LCD_readBuff[0];
    	I2C_DRV_MasterReceiveDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, (const uint8_t*)cmdFLG, cmdCnt, LCD_readBuff, cmdCnt, 5);
    }
}

void LCD_Initialize() {

	LCD_currentColor = RED;

	cmdCnt = 1;
	cmdFLG[0] = 0x10;
	cmdCAP[0] = 0x0E;

	uint8_t LCD_txBuff[LCD_DATA_LENGTH] = {0, 0x1F,0, 0,0, 0x1F,0, 0x1F,0, 0x1F,0, 0,0, 0x1F,0, 0,0, 0,0, 0xC0,0x1};

	count = LCD_DATA_LENGTH;
	LCD_modeSelected = false;
	LCD_buttonJustPressed = false;

    // Master send 1 byte CMD and data to slave
    I2C_DRV_MasterSendDataBlocking(BOARD_I2C_COMM_INSTANCE, &deviceLCD, NULL, 0, (const uint8_t*)LCD_txBuff, count, 5);


    LCD_SendCommand(0x2);		//3
    LCD_SendCommand(0x2);		//2
    LCD_SendCommand(0x8);		//2

    LCD_SendCommand(0x0);		//first
    LCD_SendCommand(0xc);		//display control: 1 | on | cursor | blinking

    LCD_SendCommand(0x0);		//8
    LCD_SendCommand(0x6);		//0
    LCD_SendCommand(0x0);		//8
    LCD_SendCommand(0x2);		//0

    LCD_SendCommand(0x0);		//0
    LCD_SendCommand(0x1);		//0
	LCD_ChangeColor(RED);
	LCD_SendWord("Use That ======>");
}
