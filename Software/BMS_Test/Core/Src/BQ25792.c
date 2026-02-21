/* bq25792.c
 *
 * STM32 HAL I2C register access helpers for TI BQ25792.
 *
 * Assumptions (adjust if your project differs):
 *  - I2C uses HAL_I2C_Mem_Read / HAL_I2C_Mem_Write
 *  - Registers are addressed by 8-bit register offset
 *  - 16-bit registers are stored LSB at reg, MSB at reg+1 (common for TI chargers)
 *    -> If your scope shows the opposite, flip BQ25792_WORD_LITTLE_ENDIAN to 0.
 */

#include "bq25792.h"
#include "stm32f4xx_hal.h"   


#ifndef BQ25792_I2C_TIMEOUT_MS
#define BQ25792_I2C_TIMEOUT_MS   50u
#endif

#ifndef BQ25792_WORD_LITTLE_ENDIAN
#define BQ25792_WORD_LITTLE_ENDIAN  1u
#endif

BQ25792_HandleTypeDef hbms;



static inline uint16_t bq25792_addr8_to_hal(uint8_t addr7)
{
    return (uint16_t)(addr7 << 1); /* HAL expects 8-bit address */
}



void bq25792_Init()
{
	hbms.ce_pin= GPIO_PIN_12;
	hbms.hi2c= &hi2c1;
	hbms.ce_port = GPIOF;
}

HAL_StatusTypeDef bq25792_read_u8(BQ25792_HandleTypeDef *hbms, uint8_t reg, uint8_t *val)
{
    if (!hbms->hi2c || !val) return HAL_ERROR;
    return HAL_I2C_Mem_Read(
        hbms->hi2c,
        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
        reg,
        I2C_MEMADD_SIZE_8BIT,
        val,
        1,
        BQ25792_I2C_TIMEOUT_MS
    );
}

HAL_StatusTypeDef bq25792_write_u8(BQ25792_HandleTypeDef *hbms, uint8_t reg, uint8_t val)
{
    if (!hbms->hi2c) return HAL_ERROR;
    return HAL_I2C_Mem_Write(
        hbms->hi2c,
        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
        reg,
        I2C_MEMADD_SIZE_8BIT,
        &val,
        1,
        BQ25792_I2C_TIMEOUT_MS
    );
}

HAL_StatusTypeDef bq25792_read_u16(BQ25792_HandleTypeDef *hbms, uint8_t reg, uint16_t *val)
{
    if (!hbms->hi2c || !val) return HAL_ERROR;

    uint8_t b[2] = {0, 0};
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(
        hbms->hi2c,
        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
        reg,
        I2C_MEMADD_SIZE_8BIT,
        b,
        2,
        BQ25792_I2C_TIMEOUT_MS
    );
    if (st != HAL_OK) return st;

#if BQ25792_WORD_LITTLE_ENDIAN
    *val = (uint16_t)b[0] | ((uint16_t)b[1] << 8);
#else
    *val = (uint16_t)b[1] | ((uint16_t)b[0] << 8);
#endif
    return HAL_OK;
}

HAL_StatusTypeDef bq25792_write_u16(BQ25792_HandleTypeDef *hbms, uint8_t reg, uint16_t val)
{
    if (!hbms->hi2c) return HAL_ERROR;

    uint8_t b[2];
#if BQ25792_WORD_LITTLE_ENDIAN
    b[0] = (uint8_t)(val & 0xFF);
    b[1] = (uint8_t)((val >> 8) & 0xFF);
#else
    b[0] = (uint8_t)((val >> 8) & 0xFF);
    b[1] = (uint8_t)(val & 0xFF);
#endif

    return HAL_I2C_Mem_Write(
        hbms->hi2c,
        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
        reg,
        I2C_MEMADD_SIZE_8BIT,
        b,
        2,
        BQ25792_I2C_TIMEOUT_MS
    );
}

/* Read-modify-write for 8-bit registers */
HAL_StatusTypeDef bq25792_update_bits_u8(BQ25792_HandleTypeDef *bms, uint8_t reg, uint8_t mask, uint8_t value)
{
    uint8_t r = 0;
    HAL_StatusTypeDef st = bq25792_read_u8(bms, reg, &r);
    if (st != HAL_OK) return st;
    r = (uint8_t)((r & ~mask) | (value & mask));
    return bq25792_write_u8(bms, reg, r);
}

HAL_StatusTypeDef bq25792_probe(BQ25792_HandleTypeDef *bms)
{
    if (!bms->hi2c) return HAL_ERROR;

    return HAL_I2C_IsDeviceReady(
        bms->hi2c,
        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
        2,                      /* trials */
        BQ25792_I2C_TIMEOUT_MS
    );
}


/**
  * @brief  Sets or resets charge enable on the BMS
  *
  * @param  hbms Pointer to GPS handler struct
  * @param  enable 
  * @retval HAL status
  */
HAL_StatusTypeDef bq25792_set_charge_enable(BQ25792_HandleTypeDef *hbms, BQ25792_CHARGE_STATE enable)
{
    HAL_GPIO_WritePin(hbms->ce_port, hbms->ce_pin,  enable == CHARGE_ENABLE ? GPIO_PIN_RESET: GPIO_PIN_SET);
    return(bq25792_update_bits_u8(
        hbms,
        BQ25792_REG0F_CHG_CTRL0,
        BQ25792_REG0F_EN_CHG,
        enable == CHARGE_ENABLE ? BQ25792_REG0F_EN_CHG : 0u
    ));
}


/**
  * @brief  Checks if specified register is 16 bit or 8 bit.
  *
  * @param  reg Register to check
  *
  * @retval True if register is a 16-bit register. False if 8-bit.
  */
bool bq25792_reg_is_16bit(uint8_t reg)
{
    switch (reg)
    {
        /* All the 16-bit registers */
        case BQ25792_REG01_VREG:
        case BQ25792_REG03_ICHG:
        case BQ25792_REG06_IINDPM:
        case BQ25792_REG0B_VOTG:
        case BQ25792_REG19_ICO_ILIM:
        case BQ25792_REG31_IBUS_ADC:
        case BQ25792_REG33_IBAT_ADC:
        case BQ25792_REG35_VBUS_ADC:
        case BQ25792_REG37_VAC1_ADC:
        case BQ25792_REG39_VAC2_ADC:
        case BQ25792_REG3B_VBAT_ADC:
        case BQ25792_REG3D_VSYS_ADC:
        case BQ25792_REG3F_TS_ADC:
        case BQ25792_REG41_TDIE_ADC:
        case BQ25792_REG43_DP_ADC:
        case BQ25792_REG45_DM_ADC:

            return true;
        default: //Rest of the registers are false
            return false;
    }
}
