/*
 * PIXY.h
 *
 *  Created on: Nov 22, 2015
 *      Author: ulab
 */

#ifndef SOURCES_PIXY_H_
#define SOURCES_PIXY_H_

Direction directionToGo;
uint32_t bytesRemaining;

void I2C_Initialize(void);
void Pixy_Init(void);
Direction Pixy_GetDirection(void);
Direction Pixy_Analyze(void);



#endif /* SOURCES_PIXY_H_ */
