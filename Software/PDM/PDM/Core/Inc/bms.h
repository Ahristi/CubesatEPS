/*
 * bms.h
 *
 *  Created on: Apr 11, 2026
 *      Author: Tom
 */

#ifndef INC_BMS_H_
#define INC_BMS_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "i2c.h"
#include <math.h>
#include <stdbool.h>


/* ------------------------------- I2C address ------------------------------ */
#define BMS_I2C_ADDR_7BIT         (0x6B)  /* target address (7-bit) */ /* :contentReference[oaicite:2]{index=2} */

/* ------------------------------ Register map ------------------------------ */
#define BMS_REG10_CHG_CTL   (0x10) //Watchdog (not currently used)
#define BMS_REG31_I_SYS     (0x31) //System current measurement
#define BMS_REG33_I_BATT    (0x33) //Battery current measurement
#define BMS_REG35_V_UMB     (0x35) //Umbilical voltage measurement
#define BMS_REG37_V_PANEL0  (0x37) //MPPT0 voltage measurement (NOTE: On the BQ25792 this is VAC1 measurement)
#define BMS_REG39_V_PANEL1  (0x39) //MPPT1 voltage measurement (NOTE: On the BQ25792 this is VAC2 measurement)
#define BMS_REG3B_V_BATT    (0x3B) //Battery Voltage
#define BMS_REG3D_V_SYS     (0x3D) //System voltage measurement
#define BMS_REG3F_T_BATT    (0x3F) //Battery Temperature
#define BMS_REG41_DIE_TEMP  (0x41) //MCU die temperature
#define BMS_REG43_I_PANEL0  (0x43) //MPPT0 current measurement (NOTE: On the BQ25792 this is D+ measurement)
#define BMS_REG45_I_PANEL1  (0x45) //MPPT1 current measurement (NOTE: On the BQ25792 this is D- measurement)

/* ----------------------- Typedefs and Enums ----------------------- */
typedef float (*BMS_convertFunction)(uint16_t);

typedef struct{
	uint8_t address;
    uint16_t raw;
    float value;
    BMS_convertFunction convert;
}BMS_Measurement_t;


typedef enum {
    BMS_PANEL0_VMON = 0,
    BMS_PANEL0_IMON,
    BMS_PANEL1_VMON,
    BMS_PANEL1_IMON,
    BMS_SYS_VMON,
    BMS_SYS_IMON,
    BMS_BAT_TMON,
    BMS_UMB_VMON,
    BMS_BATT_IMON,
    BMS_BATT_VMON,
    BMS_NUM_MEASUREMENTS
} BMS_MeasurementIndex_t;
extern BMS_Measurement_t bms_measurements[BMS_NUM_MEASUREMENTS];

typedef struct {
	I2C_HandleTypeDef* hi2c;

}BMS_handlerTypedef;
extern BMS_handlerTypedef hbms;

/* ----------------------- Function Prototypes ----------------------- */
void BMS_init(I2C_HandleTypeDef* hi2c);
void BMS_read_u8(BMS_handlerTypedef* hbms, uint8_t reg, uint8_t* val);
void BMS_write_u8(BMS_handlerTypedef* hbms, uint8_t reg, uint8_t val);
void BMS_read_u16(BMS_handlerTypedef* hbms, uint8_t reg, uint16_t* val);
void BMS_write_u16(BMS_handlerTypedef* hbms, uint8_t reg, uint16_t val);

void BMS_readBMS();

float BMS_convertPanelVoltage(uint16_t raw);
float BMS_convertPanelCurrent(uint16_t raw);
float BMS_convertSysVoltage(uint16_t raw);
float BMS_convertSysCurrent(uint16_t raw);
float BMS_convertBatteryTemperature(uint16_t raw);
float BMS_convertUmbilicalVoltage(uint16_t raw);
float BMS_convertBatteryCurrent(uint16_t raw);
float BMS_convertBatteryVoltage(uint16_t raw);
float BMS_convertDieTemperature(uint16_t raw);
float BMS_UnpackmVbit(uint16_t raw);
float BMS_UnpackmAbit(uint16_t raw);
float BMS_UnpackSignedmAbit(uint16_t raw);
float BMS_UnpackCelsiusbit(uint16_t raw);





#endif /* INC_BMS_H_ */
