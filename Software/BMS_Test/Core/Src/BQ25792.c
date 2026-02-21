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
#include "stm32xxxx_hal.h"   /* <-- change to your STM32 family header */

#ifndef BQ25792_I2C_TIMEOUT_MS
#define BQ25792_I2C_TIMEOUT_MS   50u
#endif

#ifndef BQ25792_WORD_LITTLE_ENDIAN
#define BQ25792_WORD_LITTLE_ENDIAN  1u
#endif

/* If you prefer not to pass hi2c everywhere, you can wrap these in a handle struct.
 * For quick test code, simple functions are often fastest.
 */

static inline uint16_t bq25792_addr8_to_hal(uint8_t addr7)
{
    return (uint16_t)(addr7 << 1); /* HAL expects 8-bit address */
}

HAL_StatusTypeDef bq25792_read_u8(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *val)
{
    if (!hi2c || !val) return HAL_ERROR;
    return HAL_I2C_Mem_Read(
        hi2c,
        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
        reg,
        I2C_MEMADD_SIZE_8BIT,
        val,
        1,
        BQ25792_I2C_TIMEOUT_MS
    );
}

HAL_StatusTypeDef bq25792_write_u8(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t val)
{
    if (!hi2c) return HAL_ERROR;
    return HAL_I2C_Mem_Write(
        hi2c,
        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
        reg,
        I2C_MEMADD_SIZE_8BIT,
        &val,
        1,
        BQ25792_I2C_TIMEOUT_MS
    );
}

HAL_StatusTypeDef bq25792_read_u16(I2C_HandleTypeDef *hi2c, uint8_t reg, uint16_t *val)
{
    if (!hi2c || !val) return HAL_ERROR;

    uint8_t b[2] = {0, 0};
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(
        hi2c,
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

HAL_StatusTypeDef bq25792_write_u16(I2C_HandleTypeDef *hi2c, uint8_t reg, uint16_t val)
{
    if (!hi2c) return HAL_ERROR;

    uint8_t b[2];
#if BQ25792_WORD_LITTLE_ENDIAN
    b[0] = (uint8_t)(val & 0xFF);
    b[1] = (uint8_t)((val >> 8) & 0xFF);
#else
    b[0] = (uint8_t)((val >> 8) & 0xFF);
    b[1] = (uint8_t)(val & 0xFF);
#endif

    return HAL_I2C_Mem_Write(
        hi2c,
        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
        reg,
        I2C_MEMADD_SIZE_8BIT,
        b,
        2,
        BQ25792_I2C_TIMEOUT_MS
    );
}

/* Read-modify-write for 8-bit registers */
HAL_StatusTypeDef bq25792_update_bits_u8(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t mask, uint8_t value)
{
    uint8_t r = 0;
    HAL_StatusTypeDef st = bq25792_read_u8(hi2c, reg, &r);
    if (st != HAL_OK) return st;

    r = (uint8_t)((r & ~mask) | (value & mask));
    return bq25792_write_u8(hi2c, reg, r);
}

/* Optional: a quick "is device alive" probe using HAL_I2C_IsDeviceReady */
HAL_StatusTypeDef bq25792_probe(I2C_HandleTypeDef *hi2c)
{
    if (!hi2c) return HAL_ERROR;

    return HAL_I2C_IsDeviceReady(
        hi2c,
        bq25792_addr8_to_hal(BQ25792_I2C_ADDR_7BIT),
        2,                      /* trials */
        BQ25792_I2C_TIMEOUT_MS
    );
}

/* Optional: example helper to enable charging (REG0F bit5 EN_CHG) */
HAL_StatusTypeDef bq25792_set_charge_enable(I2C_HandleTypeDef *hi2c, uint8_t enable)
{
    return bq25792_update_bits_u8(
        hi2c,
        BQ25792_REG0F_CHG_CTRL0,
        BQ25792_REG0F_EN_CHG,
        enable ? BQ25792_REG0F_EN_CHG : 0u
    );
}

/* Optional: example helper to configure core limits quickly */
HAL_StatusTypeDef bq25792_quick_config(
    I2C_HandleTypeDef *hi2c,
    uint16_t vsysmin_mv,
    uint16_t vreg_mv,
    uint16_t ichg_ma,
    uint16_t vindpm_mv,
    uint16_t iindpm_ma
)
{
    HAL_StatusTypeDef st;

    st = bq25792_write_u8(hi2c,  BQ25792_REG00_VSYSMIN, bq25792_encode_vsysmin_mv(vsysmin_mv));
    if (st != HAL_OK) return st;

    st = bq25792_write_u16(hi2c, BQ25792_REG01_VREG,    bq25792_encode_vreg_mv(vreg_mv));
    if (st != HAL_OK) return st;

    st = bq25792_write_u16(hi2c, BQ25792_REG03_ICHG,    bq25792_encode_ichg_ma(ichg_ma));
    if (st != HAL_OK) return st;

    st = bq25792_write_u8(hi2c,  BQ25792_REG05_VINDPM,  bq25792_encode_vindpm_mv(vindpm_mv));
    if (st != HAL_OK) return st;

    st = bq25792_write_u16(hi2c, BQ25792_REG06_IINDPM,  bq25792_encode_iindpm_ma(iindpm_ma));
    if (st != HAL_OK) return st;

    return HAL_OK;
}

/* --------- Minimal example usage (drop into your bring-up code) ----------
 *
 *  if (bq25792_probe(&hi2c1) == HAL_OK) {
 *      bq25792_quick_config(&hi2c1, 9000, 8400, 1000, 12000, 2000);
 *      bq25792_set_charge_enable(&hi2c1, 1);
 *
 *      uint16_t vbat_adc;
 *      bq25792_read_u16(&hi2c1, BQ25792_REG3B_VBAT_ADC, &vbat_adc);
 *  }
 */
