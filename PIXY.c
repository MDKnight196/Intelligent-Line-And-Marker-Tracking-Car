/*
Author: Zack Rauen
 */
///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

 // Standard C Included Files
// SDK Included Files
#include "board.h"
#include "fsl_os_abstraction.h"
#include "fsl_i2c_master_driver.h"
#include "DataTypes.h"
#include "PIXY.h"
#include "Common.h"

#define DATA_LENGTH             64


// Buffer store data to send to slave
uint8_t txBuff[DATA_LENGTH] = {0};
// Buffer store data to receive from slave
uint8_t readBuff[DATA_LENGTH] = {0};

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
uint32_t pixyDataCount = 34;
uint32_t i = 0;
i2c_device_t devicePIXY =
{
  .address = 0x54,
  .baudRate_kbps = 100   // 400 Kbps
};


void I2C_Initialize(void){
      SIM_SCGC4 |= SIM_SCGC4_I2C0_MASK;        // I2C0 Clock Gate Control
      SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;     // Port E Clock Gate Control
      I2C0_C1 |= I2C_C1_IICEN_MASK;
      //ALT5?
      PORTE_PCR24 = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK | PORT_PCR_ODE_MASK; // PTC1 ALT4// DSE = 1, High drive strength
      PORTE_PCR25 = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK | PORT_PCR_ODE_MASK; // PTC1 ALT4// DSE = 1, High drive strength
      I2C_DRV_MasterInit(BOARD_I2C_COMM_INSTANCE, &I2C_Master);
}

void Pixy_Init(void) {
    I2C_Initialize();        // Initialize I2C
    int count = 6;
    uint8_t tx[6] = {0,0xff,  0x58,0x2,  0x26,0x2};
    I2C_DRV_MasterSendDataBlocking(BOARD_I2C_COMM_INSTANCE, &devicePIXY, NULL, 0, (const uint8_t*)tx, count, 5);
}



/*!
* @brief The i2c master
* The function send array to slave, then receive a array from slave
* Then compare whether the two buffers are same
*/
Direction Pixy_GetDirection(void)
{
//	Pixy_Init();
//	I2C_DRV_MasterInit(BOARD_I2C_COMM_INSTANCE, &master);

	// Number byte data will be transfer


	// i2c master state

	// Number byte remaining
	// i2c device configuration

    // Initialize i2c master

        // Clear rxBuff
        for(i = 0; i < pixyDataCount; i++)
        {
            readBuff[i] = 0;
        }

	   // Master receive count byte data from slave
	   I2C_DRV_MasterReceiveDataBlocking(BOARD_I2C_COMM_INSTANCE, &devicePIXY, NULL, 0, readBuff, pixyDataCount,5);
	   // Wait until finish receive
//	   while(I2C_DRV_MasterGetReceiveStatus(BOARD_I2C_COMM_INSTANCE, &bytesRemaining) != kStatus_I2C_Success) {}
//    Pixy_Analyze();
//    I2C_DRV_MasterDeinit(BOARD_I2C_COMM_INSTANCE);
	   return Pixy_Analyze();
}
Direction Pixy_Analyze(void) {
	   int r[pixyDataCount/2];
	   int j=0;
 	   bool forward = false;

 	   for (int i=0; i < pixyDataCount; i++) {
 		   if ((readBuff[i] << 8) + readBuff[i+1] == 0xaa56 && (readBuff[i-2] << 8) + readBuff[i-2] == 0xaa55) {
 			   forward = true;
 			   break;
 		   }
 	   }

 	   for (int i=0; i < pixyDataCount/2; i++) {
 		   if (forward == false) {
 			   r[i]=(readBuff[j+1] << 8) + readBuff[j];
 		   }
 		   else {
 			  r[i]=(readBuff[j] << 8) + readBuff[j+1];
 		   }

 		   j=j+2;
 	   }

//       if (r[0] != 0xaa56 && r[1] != 0xaa56 && r[2] != 0xaa56) {
//           I2C_DRV_MasterReceiveDataBlocking(BOARD_I2C_COMM_INSTANCE, &device, NULL, 0, rxBuff, 1, 5);
//           // Wait until finish receive
////           while(I2C_DRV_MasterGetReceiveStatus(BOARD_I2C_COMM_INSTANCE, &bytesRemaining) != kStatus_I2C_Success) {}
//
//           I2C_DRV_MasterReceiveDataBlocking(BOARD_I2C_COMM_INSTANCE, &device, NULL, 0, rxBuff, pixyDataCount, 5);
//           // Wait until finish receive
////           while(I2C_DRV_MasterGetReceiveStatus(BOARD_I2C_COMM_INSTANCE, &bytesRemaining) != kStatus_I2C_Success) {}
//       }

	   Block data[2];

	   for (int j=0; j<2; j++) {
		   data[j].checksum = 0;
		   data[j].signature = 0;
		   data[j].x = 0;
		   data[j].y = 0;
		   data[j].width = 0;
		   data[j].height = 0;
		   data[j].angle = 0;
	   }
	   j=0;
	   for (int i=0; i < pixyDataCount/2; i++) {
		   if (r[i] == 0xaa56) {
			   data[j].checksum = r[i+1];
			   data[j].signature = r[i+2];
			   data[j].x = r[i+3];
			   data[j].y = r[i+4];
			   data[j].width = r[i+5];
			   data[j].height = r[i+6];
			   data[j].angle = r[i+7];
			   j++;
		   }
	   }


//       printf("\r\nFound signature %X at position %d\r\n", data[0].signature, data[0].x);
//       printf("Found signature %X at position %d\r\n", data[1].signature, data[1].x);


     Direction dir = NONE;


		 if (data[0].signature == data[1].signature && abs(data[0].x - data[1].x) > 5){  //Same signatures
			 if (data[0].signature == 0xB && data[1].signature == 0xB) { //Both posts sig B
				 dir = BACKWARD;
			 }
			 else {        //Both posts sig A
				 dir = FORWARD;
			 }
		 }

		 else {
			 if (data[0].signature == 0xa && data[1].signature == 0xb) {
				 if (data[0].x < data[1].x) {
					 // AB
					 dir = RIGHT;
				 }
				 else {
					 // BA
					 dir = LEFT;
				 }
			 }
			 else if (data[0].signature == 0xb && data[1].signature == 0xa) {
				 if (data[0].x < data[1].x) {
					 // BA
					 dir = LEFT;
				 }
				 else {
					 // AB
					 dir = RIGHT;
				 }
			 }
		 }

 directionToGo = dir;
 return dir;
}
