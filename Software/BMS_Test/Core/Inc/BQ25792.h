/* bq25792.h
 *
 * Minimal register/bitfield header for TI BQ25792 buck-boost battery charger.
 * Datasheet: BQ25792 (Rev. C). 7-bit I2C address = 0x6B. :contentReference[oaicite:0]{index=0}
 *
 * Notes:
 *  - Register offsets come from Table 9-12 “I2C Registers”. :contentReference[oaicite:1]{index=1}
 *  - Some registers are 8-bit, some are 16-bit (read/write as little-endian if your
 *    I2C helpers do that; otherwise follow your platform convention).
 *  - This header is intended to get you running quickly for bring-up / test code.
 */

#ifndef BQ25792_H
#define BQ25792_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------- I2C address ------------------------------ */
#define BQ25792_I2C_ADDR_7BIT         (0x6B)  /* target address (7-bit) */ /* :contentReference[oaicite:2]{index=2} */

/* ------------------------------ Bit helpers -------------------------------- */
#define BQ25792_BIT(n)                (1u << (n))
#define BQ25792_FIELD_MASK(width)     ((1u << (width)) - 1u)
#define BQ25792_FIELD_PREP(mask, val) (((uint16_t)(val) << (__builtin_ctz(mask))) & (mask))
#define BQ25792_FIELD_GET(mask, reg)  (((uint16_t)(reg) & (mask)) >> (__builtin_ctz(mask)))

/* ------------------------------ Register map ------------------------------ */
/* Offsets from Table 9-12. :contentReference[oaicite:3]{index=3} */
#define BQ25792_REG00_VSYSMIN                 (0x00) /* REG00_Minimal_System_Voltage (8-bit) */
#define BQ25792_REG01_VREG                    (0x01) /* REG01_Charge_Voltage_Limit (16-bit) */
#define BQ25792_REG03_ICHG                    (0x03) /* REG03_Charge_Current_Limit (16-bit) */
#define BQ25792_REG05_VINDPM                  (0x05) /* REG05_Input_Voltage_Limit (8-bit) */
#define BQ25792_REG06_IINDPM                  (0x06) /* REG06_Input_Current_Limit (16-bit) */
#define BQ25792_REG08_PRECHG                  (0x08) /* REG08_Precharge_Control (8-bit) */
#define BQ25792_REG09_TERM                    (0x09) /* REG09_Termination_Control (8-bit) */
#define BQ25792_REG0A_RECHG                   (0x0A) /* REG0A_Re-charge_Control (8-bit) */
#define BQ25792_REG0B_VOTG                    (0x0B) /* REG0B_VOTG_regulation (16-bit) */
#define BQ25792_REG0D_IOTG                    (0x0D) /* REG0D_IOTG_regulation (8-bit) */
#define BQ25792_REG0E_TIMER                   (0x0E) /* REG0E_Timer_Control (8-bit) */
#define BQ25792_REG0F_CHG_CTRL0               (0x0F) /* REG0F_Charger_Control_0 (8-bit) */
#define BQ25792_REG10_CHG_CTRL1               (0x10) /* REG10_Charger_Control_1 */
#define BQ25792_REG11_CHG_CTRL2               (0x11) /* REG11_Charger_Control_2 */
#define BQ25792_REG12_CHG_CTRL3               (0x12) /* REG12_Charger_Control_3 */
#define BQ25792_REG13_CHG_CTRL4               (0x13) /* REG13_Charger_Control_4 */
#define BQ25792_REG14_CHG_CTRL5               (0x14) /* REG14_Charger_Control_5 */
#define BQ25792_REG15_RESERVED                (0x15)
#define BQ25792_REG16_TEMP_CTRL               (0x16)
#define BQ25792_REG17_NTC_CTRL0               (0x17)
#define BQ25792_REG18_NTC_CTRL1               (0x18)
#define BQ25792_REG19_ICO_ILIM                (0x19)

#define BQ25792_REG1B_STATUS0                 (0x1B)
#define BQ25792_REG1C_STATUS1                 (0x1C)
#define BQ25792_REG1D_STATUS2                 (0x1D)
#define BQ25792_REG1E_STATUS3                 (0x1E)
#define BQ25792_REG1F_STATUS4                 (0x1F)

#define BQ25792_REG20_FAULT0                  (0x20)
#define BQ25792_REG21_FAULT1                  (0x21)

#define BQ25792_REG22_FLAG0                   (0x22)
#define BQ25792_REG23_FLAG1                   (0x23)
#define BQ25792_REG24_FLAG2                   (0x24)
#define BQ25792_REG25_FLAG3                   (0x25)

#define BQ25792_REG26_FAULT_FLAG0             (0x26)
#define BQ25792_REG27_FAULT_FLAG1             (0x27)

#define BQ25792_REG28_MASK0                   (0x28)
#define BQ25792_REG29_MASK1                   (0x29)
#define BQ25792_REG2A_MASK2                   (0x2A)
#define BQ25792_REG2B_MASK3                   (0x2B)
#define BQ25792_REG2C_FAULT_MASK0             (0x2C)
#define BQ25792_REG2D_FAULT_MASK1             (0x2D)

#define BQ25792_REG2E_ADC_CTRL                (0x2E) /* REG2E_ADC_Control :contentReference[oaicite:4]{index=4} */
#define BQ25792_REG2F_ADC_DIS0                (0x2F) /* REG2F_ADC_Function_Disable_0 :contentReference[oaicite:5]{index=5} */
#define BQ25792_REG30_ADC_DIS1                (0x30) /* REG30_ADC_Function_Disable_1 :contentReference[oaicite:6]{index=6} */

#define BQ25792_REG31_IBUS_ADC                (0x31)
#define BQ25792_REG33_IBAT_ADC                (0x33)
#define BQ25792_REG35_VBUS_ADC                (0x35)
#define BQ25792_REG37_VAC1_ADC                (0x37)
#define BQ25792_REG39_VAC2_ADC                (0x39)
#define BQ25792_REG3B_VBAT_ADC                (0x3B)
#define BQ25792_REG3D_VSYS_ADC                (0x3D)
#define BQ25792_REG3F_TS_ADC                  (0x3F)
#define BQ25792_REG41_TDIE_ADC                (0x41)
#define BQ25792_REG43_DP_ADC                  (0x43)
#define BQ25792_REG45_DM_ADC                  (0x45)

#define BQ25792_REG47_DPDM_DRIVER             (0x47)
#define BQ25792_REG48_PART_INFO               (0x48)

/* ----------------------- Selected bitfields / masks ----------------------- */

/* REG0F Charger Control 0 (Offset 0x0F) :contentReference[oaicite:7]{index=7} */
#define BQ25792_REG0F_EN_AUTO_IBATDIS         BQ25792_BIT(7)
#define BQ25792_REG0F_FORCE_IBATDIS           BQ25792_BIT(6)
#define BQ25792_REG0F_EN_CHG                  BQ25792_BIT(5)
#define BQ25792_REG0F_EN_ICO                  BQ25792_BIT(4)
#define BQ25792_REG0F_FORCE_ICO               BQ25792_BIT(3)
#define BQ25792_REG0F_EN_HIZ                  BQ25792_BIT(2)
#define BQ25792_REG0F_EN_TERM                 BQ25792_BIT(1)
/* bit0 reserved */

/* REG2E ADC Control (Offset 0x2E) :contentReference[oaicite:8]{index=8} */
#define BQ25792_REG2E_ADC_EN                  BQ25792_BIT(7)
#define BQ25792_REG2E_ADC_RATE                BQ25792_BIT(6)          /* 0=continuous, 1=one-shot */
#define BQ25792_REG2E_ADC_SAMPLE_MASK         (0x3u << 4)             /* bits[5:4] */
#define BQ25792_REG2E_ADC_AVG                 BQ25792_BIT(3)
#define BQ25792_REG2E_ADC_AVG_INIT            BQ25792_BIT(2)
/* bits[1:0] reserved */

/* REG2F ADC Function Disable 0 (Offset 0x2F) :contentReference[oaicite:9]{index=9} */
#define BQ25792_REG2F_IBUS_ADC_DIS            BQ25792_BIT(7)
#define BQ25792_REG2F_IBAT_ADC_DIS            BQ25792_BIT(6)
#define BQ25792_REG2F_VBUS_ADC_DIS            BQ25792_BIT(5)
#define BQ25792_REG2F_VBAT_ADC_DIS            BQ25792_BIT(4)
#define BQ25792_REG2F_VSYS_ADC_DIS            BQ25792_BIT(3)
#define BQ25792_REG2F_TS_ADC_DIS              BQ25792_BIT(2)
#define BQ25792_REG2F_TDIE_ADC_DIS            BQ25792_BIT(1)
/* bit0 reserved */

/* REG30 ADC Function Disable 1 (Offset 0x30) :contentReference[oaicite:10]{index=10} */
#define BQ25792_REG30_DP_ADC_DIS              BQ25792_BIT(7)
#define BQ25792_REG30_DM_ADC_DIS              BQ25792_BIT(6)
#define BQ25792_REG30_VAC2_ADC_DIS            BQ25792_BIT(5)
#define BQ25792_REG30_VAC1_ADC_DIS            BQ25792_BIT(4)
/* bits[3:0] reserved */


/*------------------------ BMS Handler ------------------------------------*/

typedef struct {
	I2C_HandleTypeDef *hi2c;

	GPIO_TypeDef *ce_port;
	uint16_t ce_pin;
};



/* ----------------------- Quick encode/decode helpers ---------------------- */
/* These follow the datasheet “Fixed Offset” and “Bit Step Size” fields. */

static inline uint8_t  bq25792_encode_vsysmin_mv(uint16_t mv)
{
    /* REG00 VSYSMIN: offset=2500mV, step=250mV, range 2500–16000mV :contentReference[oaicite:11]{index=11} */
    if (mv < 2500) mv = 2500;
    if (mv > 16000) mv = 16000;
    return (uint8_t)((mv - 2500) / 250);
}

static inline uint16_t bq25792_encode_vreg_mv(uint16_t mv)
{
    /* REG01 VREG: step=10mV, range 3000–18800mV :contentReference[oaicite:12]{index=12} */
    if (mv < 3000) mv = 3000;
    if (mv > 18800) mv = 18800;
    return (uint16_t)(mv / 10);
}

static inline uint16_t bq25792_encode_ichg_ma(uint16_t ma)
{
    /* REG03 ICHG: step=10mA, range 50–5000mA :contentReference[oaicite:13]{index=13} */
    if (ma < 50) ma = 50;
    if (ma > 5000) ma = 5000;
    return (uint16_t)(ma / 10);
}

static inline uint8_t  bq25792_encode_vindpm_mv(uint16_t mv)
{
    /* REG05 VINDPM: step=100mV, range 3600–22000mV :contentReference[oaicite:14]{index=14} */
    if (mv < 3600) mv = 3600;
    if (mv > 22000) mv = 22000;
    return (uint8_t)(mv / 100);
}

static inline uint16_t bq25792_encode_iindpm_ma(uint16_t ma)
{
    /* REG06 IINDPM: step=10mA, range 100–3300mA :contentReference[oaicite:15]{index=15} */
    if (ma < 100) ma = 100;
    if (ma > 3300) ma = 3300;
    return (uint16_t)(ma / 10);
}

static inline uint16_t bq25792_encode_votg_mv(uint16_t mv)
{
    /* REG0B VOTG: offset=2800mV, step=10mV, range 2800–22000mV :contentReference[oaicite:16]{index=16} */
    if (mv < 2800) mv = 2800;
    if (mv > 22000) mv = 22000;
    return (uint16_t)((mv - 2800) / 10);
}

static inline uint8_t  bq25792_encode_iotg_ma(uint16_t ma)
{
    /* REG0D IOTG: step=40mA, range 120–3320mA :contentReference[oaicite:17]{index=17} */
    if (ma < 120) ma = 120;
    if (ma > 3320) ma = 3320;
    return (uint8_t)(ma / 40);
}

#ifdef __cplusplus
}
#endif

#endif /* BQ25792_H */
