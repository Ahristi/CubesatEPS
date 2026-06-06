/*
 * bms.c
 *
 *  Created on: May 31, 2026
 *      Author: Tom Holland
 */
#include "bms.h"
BMS_handlerTypedef hbms;
#ifndef BMS_I2C_TIMEOUT_MS
#define BMS_I2C_TIMEOUT_MS   50u
#endif

#ifndef BMS_WORD_LITTLE_ENDIAN
#define BMS_WORD_LITTLE_ENDIAN  1u
#endif



BMS_Measurement_t bms_measurements[BMS_NUM_MEASUREMENTS] = {
    [BMS_PANEL0_VMON] = {
        .raw = 0,
		.address = BMS_REG37_V_PANEL0,
        .value = 0.0f,
        .convert = BMS_UnpackmVbit
    },

    [BMS_PANEL0_IMON] = {
		.raw = 0,
		.address = BMS_PANEL0_IMON,
        .value = 0.0f,
        .convert = BMS_UnpackmAbit
    },

    [BMS_PANEL1_VMON] = {
        .raw = 0,
		.address = BMS_REG39_V_PANEL1,
        .value = 0.0f,
        .convert = BMS_UnpackmVbit
    },

    [BMS_PANEL1_IMON] = {
        .raw = 0,
		.address = BMS_REG45_I_PANEL1,
        .value = 0.0f,
        .convert = BMS_UnpackmAbit
    },

    [BMS_SYS_VMON] = {
        .raw = 0,
        .address = BMS_REG3D_V_SYS,
        .value = 0.0f,
        .convert = BMS_UnpackmVbit
    },

    [BMS_SYS_IMON] = {
        .raw = 0,
        .address = BMS_REG31_I_SYS,
        .value = 0.0f,
        .convert = BMS_UnpackmAbit
    },

    [BMS_BAT_TMON] = {
        .raw = 0,
        .address = BMS_REG3F_T_BATT,
        .value = 0.0f,
        .convert = BMS_UnpackCelsiusbit
    },

    [BMS_UMB_VMON] = {
        .raw = 0,
        .address = BMS_REG35_V_UMB,
        .value = 0.0f,
        .convert = BMS_UnpackmVbit
    },

    [BMS_BATT_VMON] = {
        .raw = 0,
        .address = BMS_REG3B_V_BATT,
        .value = 0.0f,
        .convert = BMS_UnpackmVbit
    },

    [BMS_BATT_IMON] = {
        .raw = 0,
        .address = BMS_REG33_I_BATT,
        .value = 0.0f,
        .convert = BMS_UnpackSignedmAbit
    }
};


static inline uint16_t BMS_addr8_to_hal(uint8_t addr7)
{
    return (uint16_t)(addr7 << 1); /* HAL expects 8-bit address */
}



void BMS_init(I2C_HandleTypeDef* hi2c)
{
	hbms.hi2c= hi2c;
}

void BMS_read_u8(BMS_handlerTypedef* hbms, uint8_t reg, uint8_t* val)
{
	HAL_I2C_Mem_Read(
	        hbms->hi2c,
	        BMS_addr8_to_hal(BMS_I2C_ADDR_7BIT),
	        reg,
	        I2C_MEMADD_SIZE_8BIT,
	        val,
	        1,
	        BMS_I2C_TIMEOUT_MS
	    );
}
void BMS_write_u8(BMS_handlerTypedef* hbms, uint8_t reg, uint8_t val)
{
	HAL_I2C_Mem_Write(
	        hbms->hi2c,
	        BMS_addr8_to_hal(BMS_I2C_ADDR_7BIT),
	        reg,
	        I2C_MEMADD_SIZE_8BIT,
	        &val,
	        1,
	        BMS_I2C_TIMEOUT_MS
	    );
}
void BMS_read_u16(BMS_handlerTypedef* hbms, uint8_t reg, uint16_t* val)
{
	uint8_t b[2] = {0, 0};
	if (HAL_I2C_Mem_Read(
	        hbms->hi2c,
	        BMS_addr8_to_hal(BMS_I2C_ADDR_7BIT),
	        reg,
	        I2C_MEMADD_SIZE_8BIT,
	        b,
	        2,
	        BMS_I2C_TIMEOUT_MS
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
	        BMS_addr8_to_hal(BMS_I2C_ADDR_7BIT),
	        reg,
	        I2C_MEMADD_SIZE_8BIT,
	        b,
	        2,
	        BMS_I2C_TIMEOUT_MS
	    );
}
void BMS_readBMS()
{
    for (uint16_t i = 0; i < BMS_NUM_MEASUREMENTS; i++)
    {
		BMS_read_u16(&hbms, bms_measurements[i].address, &bms_measurements[i].raw);
		bms_measurements[i].value = bms_measurements[i].convert(bms_measurements[i].raw);
		HAL_Delay(1); //Little delay to ensure BMS doesn't shit the bed
    }
}



float BMS_UnpackmVbit(uint16_t raw)
{
	return (float)(raw);
}
float BMS_UnpackmAbit(uint16_t raw)
{
	return (float)(raw);
}
float BMS_UnpackSignedmAbit(uint16_t raw)
{
	return (float)(raw);
}
float BMS_UnpackCelsiusbit(uint16_t raw)
{
	return (float)(raw);
}


