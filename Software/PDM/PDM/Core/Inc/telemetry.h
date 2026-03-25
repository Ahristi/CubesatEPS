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
#include <stdio.h>
#include <string.h>
//-----------DEFINES-----------

#define UART_BUFFER_LENGTH  64
#define CAN_ADDR 0x69

//----------TYPEDEF AND ENUM----------
typedef enum
{
    CMD_NONE     = 0,
    CMD_EFUSE    = 0x47,
    CMD_CONV     = 0x65,
	CMD_SYS      = 0x67,
	CMD_HEATER   = 0x25
} EPSCmdType_t;


typedef struct
{
	EPSCmdType_t type;
    uint16_t data;
} EPSCommand_t;

//----------VARIABLES----------
extern uint16_t rx_index;
extern uint8_t rx_ready;
extern uint8_t rx_byte;
extern uint8_t rx_line[UART_BUFFER_LENGTH];
extern uint8_t rx_msg_len;


void TELEMETRY_sendStartupMessage(void);
void uartCMDHandler(uint8_t *msg, uint16_t len);


#endif /* INC_TELEMETRY_H_ */
