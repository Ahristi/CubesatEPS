/*
 * telemetry.h
 *
 *  Created on: Mar 25, 2026
 *      Author: Tom Holland
 */

#ifndef INC_TELEMETRY_H_
#define INC_TELEMETRY_H_

#include <stdint.h>
#include "usart.h"
#include "eps_config.h"
#include "analog.h"
#include "can.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
//-----------DEFINES-----------

#define UART_BUFFER_LENGTH  64
#define STANDARD_TELEM_PACKETS 5
#define CAN_FRAME_LENGTH 8


//----------TYPEDEF AND ENUM----------
typedef enum
{
    CMD_NONE     = 0,
    CMD_EFUSE    = 0x47,
    CMD_CONV     = 0x65,
	CMD_SYS      = 0x67,
	CMD_HEATER   = 0x25
} EPSCmdType_t;


typedef enum
{
    ID_3V3_MEASUREMENTS  = 0x320,
	ID_5V_MEASUREMENTS   = 0x321,
	ID_6V_MEASUREMENTS   = 0x322,
	ID_12V_MEASUREMENTS  = 0x323,
	ID_MISC_MEASUREMENTS = 0x324
} EPSTelemType_t;



typedef struct
{
	EPSCmdType_t type;
    uint16_t data;
} EPSCommand_t;


typedef struct{
	CAN_HandleTypeDef*  hcan;
	UART_HandleTypeDef* huart;
}TELEMETRY_handlerTypedef;


extern TELEMETRY_handlerTypedef htelem;



//----------VARIABLES----------
extern uint16_t rx_index;
extern uint8_t rx_ready;
extern uint8_t rx_byte;
extern uint8_t rx_line[UART_BUFFER_LENGTH];
extern uint8_t rx_msg_len;


void TELEMETRY_sendStartupMessage(void);
void TELEMETRY_Init(CAN_HandleTypeDef*  hcan, UART_HandleTypeDef* huart);
void TELEMETRY_sendCANMessage(uint32_t stdID, const uint8_t *frame_data, uint8_t dlc);
void uartCMDHandler(uint8_t *msg, uint16_t len);
#endif /* INC_TELEMETRY_H_ */
