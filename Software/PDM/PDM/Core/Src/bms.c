/*
 * bms.c
 *
 *  Created on: Apr 11, 2026
 *      Author: Tom Holland
 */
#include "bms.h"
BMS_handlerTypedef hbms;
#ifndef BQ25792_I2C_TIMEOUT_MS
#define BQ25792_I2C_TIMEOUT_MS   50u
#endif

#ifndef BQ25792_WORD_LITTLE_ENDIAN
#define BQ25792_WORD_LITTLE_ENDIAN  1u
#endif


static inline uint16_t bq25792_addr8_to_hal(uint8_t addr7)
{
    return (uint16_t)(addr7 << 1); /* HAL expects 8-bit address */
}



void BMS_init(I2C_HandleTypeDef* hi2c)
{
	hbms.hi2c= hi2c;
	//BMS_configureADCs();
}

void BMS_read_u8(BMS_handlerTypedef* hbms, uint8_t reg, uint8_t* val)
{
	HAL_I2C_Mem_Read(
	        hbms->hi2c,
	        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
	        reg,
	        I2C_MEMADD_SIZE_8BIT,
	        val,
	        1,
	        BQ25792_I2C_TIMEOUT_MS
	    );
}
void BMS_write_u8(BMS_handlerTypedef* hbms, uint8_t reg, uint8_t val)
{
	HAL_I2C_Mem_Write(
	        hbms->hi2c,
	        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
	        reg,
	        I2C_MEMADD_SIZE_8BIT,
	        &val,
	        1,
	        BQ25792_I2C_TIMEOUT_MS
	    );
}
void BMS_read_u16(BMS_handlerTypedef* hbms, uint8_t reg, uint16_t* val)
{
	uint8_t b[2] = {0, 0};
	if (HAL_I2C_Mem_Read(
	        hbms->hi2c,
	        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
	        reg,
	        I2C_MEMADD_SIZE_8BIT,
	        b,
	        2,
	        BQ25792_I2C_TIMEOUT_MS
	    ) == HAL_OK)
	{
		*val =   ((uint16_t)b[0] << 8)| (uint16_t)b[1];
	}
	else
	{
		*val = 69;
	}

}
void BMS_write_u16(BMS_handlerTypedef* hbms, uint8_t reg, uint16_t val)
{
	uint8_t b[2];
    b[0] = (uint8_t)(val & 0xFF);
    b[1] = (uint8_t)((val >> 8) & 0xFF);
	HAL_I2C_Mem_Write(
	        hbms->hi2c,
	        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
	        reg,
	        I2C_MEMADD_SIZE_8BIT,
	        b,
	        2,
	        BQ25792_I2C_TIMEOUT_MS
	    );
}


void BMS_getBatteryVoltage()
{
	uint16_t vbatt;
	BMS_read_u16(&hbms, BQ25792_REG3B_VBAT_ADC, &vbatt);
	hbms.rawBatteryVoltage = vbatt;
	hbms.batteryVoltage = vbatt/1000.0f;
}
void BMS_getBatteryTemperature()
{
	uint16_t tsScale;
	float battTemp;
	BMS_read_u16(&hbms, BQ25792_REG3F_TS_ADC, &tsScale);
	battTemp = BMS_convertBatteryTemp(tsScale);
	hbms.batteryTemp = battTemp;
	hbms.rawBatteryTemp= (int16_t)battTemp;
}

void BMS_getDieTemperature()
{
	uint16_t rawDieTemp;
	BMS_read_u16(&hbms, BQ25792_REG41_TDIE_ADC, &rawDieTemp);
	hbms.rawDieTemp = (int16_t)rawDieTemp;
	hbms.dieTemp = BMS_convertDieTemp(rawDieTemp);

}
void BMS_getBusCurrent()
{
	uint16_t rawBusCurrent;
	BMS_read_u16(&hbms, BQ25792_REG31_IBUS_ADC, &rawBusCurrent);
	hbms.rawBusCurrent = rawBusCurrent;
	hbms.busCurrent = BMS_convertBusCurrent(rawBusCurrent);
}
void BMS_getBatteryCurrent()
{
	uint16_t rawBatteryCurrent;
	BMS_read_u16(&hbms, BQ25792_REG33_IBAT_ADC, &rawBatteryCurrent);
	hbms.rawBatteryCurrent = rawBatteryCurrent;
	hbms.batteryCurrent = BMS_convertBatteryCurrent(rawBatteryCurrent);
}
void BMS_getBusVoltage()
{
	uint16_t rawBusVoltage;
	BMS_read_u16(&hbms, BQ25792_REG35_VBUS_ADC, &rawBusVoltage);
	hbms.rawBusVoltage = rawBusVoltage;
	hbms.busVoltage = BMS_convertBusVoltage(rawBusVoltage);
}
void BMS_getSysVoltage()
{
	uint16_t rawSysVoltage;
	BMS_read_u16(&hbms, BQ25792_REG3D_VSYS_ADC, &rawSysVoltage);
	hbms.rawSysVoltage = rawSysVoltage;
	hbms.sysVoltage = BMS_convertBusVoltage(rawSysVoltage);
}

void BMS_resetWatchdog()
{
	uint8_t WD_CTRL_VAL = BQ25792_REG10_VAC_OVP | BQ25792_REG10_WD_RST | BQ25792_REG10_WATCHDOG;
	BMS_write_u8(&hbms, BQ25792_REG10_CHG_CTRL1, WD_CTRL_VAL);
}

void BMS_readBMS()
{
	BMS_getBatteryVoltage();
	HAL_Delay(1);
	BMS_getBatteryTemperature();
	HAL_Delay(1);
	BMS_getDieTemperature();
	HAL_Delay(1);
	BMS_getBusCurrent();
	HAL_Delay(1);
	BMS_getBatteryCurrent();
	HAL_Delay(1);
	BMS_getBusVoltage();
	HAL_Delay(1);
	BMS_getSysVoltage();
	HAL_Delay(1);
}


void BMS_configureADCs()
{
	//Enable ADC and set rate to continuous conversion. No Averaging.
	uint8_t ADC_CTRL_VAL = BQ25792_REG2E_ADC_EN | (!BQ25792_REG2E_ADC_RATE);
	BMS_write_u8(&hbms, BQ25792_REG2E_ADC_CTRL, ADC_CTRL_VAL);
/*	Ship fet not present.
 *  IBat sensing enabled.
 *  IBAT discharge current regulation 3A
 *  Internal IINDPM register input current regulation enabled
 *  ILIM_HIZ enabled
 *  Battery discharging current OCP disabled (handled by protection IC instead)
 */
	uint8_t CHG_CTRL5_VAL =
							  (BQ25792_REG14_SFET_PRESENT)
							| (BQ25792_REG14_EN_IBAT)
							| (BQ25792_REG14_IBAT_REG)
							| (BQ25792_REG14_EN_IINDPM)
							| (BQ25792_REG14_EN_EXTILIM)
							| (BQ25792_REG14_EN_BATOC);
	BMS_write_u8(&hbms, BQ25792_REG14_CHG_CTRL5, CHG_CTRL5_VAL);
}


float BMS_convertBatteryTemp(uint16_t rawTemp)
{
	float TS_ratio = rawTemp * REG3F_TS_ADC_BIT_STEP;
	float Rp = TS_ratio * R11/(1-TS_ratio);
	float Rt = Rp *R12/(R12 - Rp);
    if (Rt <= 0.0f) return 0;

    float inv_T = (1.0f / THERM_T0) +
                  (1.0f / THERM_BETA) * logf(Rt / THERM_R0);
    float temp_C = (1.0f / inv_T) - 273.15f;
    return temp_C;
}

float BMS_convertDieTemp(uint16_t rawTemp)
{
	int16_t signedTemp = (int16_t)rawTemp;
	return signedTemp * 0.5f;
}

float BMS_convertBusCurrent(uint16_t rawCurrent)
{
	int16_t signedCurrent = (int16_t)rawCurrent;
	return signedCurrent;
}

float BMS_convertBatteryCurrent(uint16_t rawCurrent)
{
	return (float)((int16_t)rawCurrent /1000.0f);
}
float BMS_convertBusVoltage(uint16_t rawVoltage)
{
	return (float)((int16_t)rawVoltage /1000.0f);
}
float BMS_convertSysVoltage(uint16_t rawVoltage)
{
	return (float)((int16_t)rawVoltage /1000.0f);
}
