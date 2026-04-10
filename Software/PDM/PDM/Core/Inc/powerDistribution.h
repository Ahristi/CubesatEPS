/*
 * powerDistribution.h
 *
 *  Created on: Mar 25, 2026
 *      Author: Tom Holland
 */

#ifndef INC_POWERDISTRIBUTION_H_
#define INC_POWERDISTRIBUTION_H_
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "main.h"
#include "telemetry.h"

//-----------BITMASKS-------------

//eFuse enables
#define _3V3_CH2_EN_MASK  0x01
#define _5V_CH1_EN_MASK   0x02
#define _5V_CH2_EN_MASK   0x04
#define _6V_CH1_EN_MASK   0x08
#define _6V_CH2_EN_MASK   0x00 //Not used on EPS V1
#define _12V_CH1_EN_MASK  0x10
#define _12V_CH2_EN_MASK  0x20


//Converter enables
#define _5V_CONV_EN_MASK  0x01
#define _6V_CONV_EN_MASK  0x02
#define _12V_CONV_EN_MASK 0x04

//System command masks
#define POWER_SAVE_MASK   0x01
#define RESET_MASK	      0x02


//--------Common bitmask combinations-------
#define DEFAULT_CONV_STATE _5V_CONV_EN_MASK | _6V_CONV_EN_MASK | _12V_CH1_EN_MASK
#define STARTUP_CONV_STATE 0x00




typedef enum{
	DISTRIBUTION_STARTUP,		//Enable converters and configure BMS
	DISTRIBUTION_POWER_SAVE,		//Turn off all converters except 3V3 and only enable 3V3 CH1
	DISTRIBUTION_NORMAL,			//Normal operation. Turn channels on and off as commanded
	DISTRIBUTION_RESET			//Power cycle OBC if 5 minute watchdog expires
}DistributionState;


typedef struct{
	DistributionState state;

	uint16_t eFuseCommands;
	uint8_t eFuseStates;
	uint8_t eFuseFaults;
	uint8_t  eFuseCommandReceived;

	uint16_t convStates;
	uint16_t convCommands;
	uint8_t  convCommandReceived;

	uint16_t sysCommands;
	uint16_t sysFaults;

}DISTRIBUTION_HandleTypeDef;

extern QueueHandle_t distributionQueue;





extern DISTRIBUTION_HandleTypeDef hdist;
void DISTRIBUTION_Init();
void DISTRIBUTION_DisableAlleFuses(void);
void DISTRIBUTION_DisableConverters(void);
void DISTRIBUTION_UpdateState(EPS_CommandTypedef cmd);
void DISTRIBUTION_ApplyeFuseCommands(uint16_t eFuseCMD);
void DISTRIBUTION_checkEfuseFaults(void);

#endif /* INC_POWERDISTRIBUTION_H_ */
