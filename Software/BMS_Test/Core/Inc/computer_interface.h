/*
 * computer_interface.h
 *
 *  Created on: Feb 21, 2026
 *      Author: Tom
 */

#ifndef INC_COMPUTER_INTERFACE_H_
#define INC_COMPUTER_INTERFACE_H_


#define WRITE_SINGLE    0x00
#define READ_SINGLE     0x01
#define WRITE_MULTIPLE  0x02
#define READ_MULTIPLE   0x03
#define CMD_ERROR       0x07



#define READ_SINGLE_RETURN_LENGTH      3
#define WRITE_SINGLE_RETURN_LENGTH     1
#define WRITE_MULTIPLE_RETURN_LENGTH   1
#define ERROR_RETURN_LENGTH            1
//Note that read multiple doesn't have a predefined return length as it depends on the number of registers being read.




#include "BQ25792.h"
#include "usart.h"
#include "stm32f4xx_hal.h"


HAL_StatusTypeDef waitCMD(BQ25792_HandleTypeDef* hbms);
HAL_StatusTypeDef readSingle(BQ25792_HandleTypeDef* hbms, uint8_t reg);
HAL_StatusTypeDef writeSingle(BQ25792_HandleTypeDef* hbms, uint8_t reg, uint16_t val);


#endif /* INC_COMPUTER_INTERFACE_H_ */
