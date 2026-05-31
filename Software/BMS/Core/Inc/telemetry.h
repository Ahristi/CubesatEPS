#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__

#include <stdint.h>
#include "analog.h"
#include "i2c.h"
/*
    README: This code is intended to replace legacy hardware which used a texas instruments BQ25792 battery management IC.
            For more info on this IC see: https://download.mikroe.com/documents/datasheets/BQ25792_datasheet.pdf
            The code intends to mirror the functionality and API of the old BMS so that it can be directly swappable with 
            the old hardware. This explains why certain weird conversions and registers mappings are chosen.
*/








//-------------Defines-------------
#define REGISTER_MAP_SIZE 256

/* ------------------------------ Register map ------------------------------ */
#define BMS_REG10_CHG_CTL   (0x10) //Watchdog
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





//-------------Typedef and Enums-------------
typedef struct{

}TELEMETRY_Handler_t;



void TELEMETRY_Init(void);
void TELEMETRY_Task(void);
void TELEMETRY_UpdateMeasurements(void);
void TELEMETRY_WriteRegU8(uint16_t addr, uint8_t val);
void TELEMETRY_WriteRegU16(uint16_t addr, uint16_t val);
uint8_t TELEMETRY_ReadRegU8(uint16_t addr);
uint16_t TELEMETRY_ReadRegU16(uint16_t addr);




#endif