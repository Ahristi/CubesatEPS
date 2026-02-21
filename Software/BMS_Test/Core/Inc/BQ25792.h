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
#include "gpio.h"
#include "i2c.h"
#include <stdbool.h>
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
/* Offsets from Table 9-12*/
#define BQ25792_REG00_VSYSMIN       (0x00) /* REG00_Minimal_System_Voltage (8-bit) – VSYSMIN setting */
#define BQ25792_REG01_VREG          (0x01) /* REG01_Charge_Voltage_Limit (16-bit) – Battery regulation voltage limit */
#define BQ25792_REG03_ICHG          (0x03) /* REG03_Charge_Current_Limit (16-bit) – Fast charge current limit */
#define BQ25792_REG05_VINDPM        (0x05) /* REG05_Input_Voltage_Limit (8-bit) – Input voltage DPM (VINDPM) threshold */
#define BQ25792_REG06_IINDPM        (0x06) /* REG06_Input_Current_Limit (16-bit) – Input current limit (IINDPM) */
#define BQ25792_REG08_PRECHG        (0x08) /* REG08_Precharge_Control (8-bit) – Precharge current / thresholds control */
#define BQ25792_REG09_TERM          (0x09) /* REG09_Termination_Control (8-bit) – Termination current / termination enable control */
#define BQ25792_REG0A_RECHG         (0x0A) /* REG0A_Re-charge_Control (8-bit) – Recharge threshold / hysteresis control */
#define BQ25792_REG0B_VOTG          (0x0B) /* REG0B_VOTG_Regulation (16-bit) – OTG output voltage regulation setting */
#define BQ25792_REG0D_IOTG          (0x0D) /* REG0D_IOTG_Regulation (8-bit) – OTG output current limit setting */
#define BQ25792_REG0E_TIMER         (0x0E) /* REG0E_Timer_Control (8-bit) – Charge safety timer / watchdog timer control */
#define BQ25792_REG0F_CHG_CTRL0     (0x0F) /* REG0F_Charger_Control_0 (8-bit) – Charger enable / HIZ / ICO / termination control */
#define BQ25792_REG10_CHG_CTRL1     (0x10) /* REG10_Charger_Control_1 (8-bit) – Additional charger mode controls */
#define BQ25792_REG11_CHG_CTRL2     (0x11) /* REG11_Charger_Control_2 (8-bit) – Power-path / system related controls */
#define BQ25792_REG12_CHG_CTRL3     (0x12) /* REG12_Charger_Control_3 (8-bit) – Boost/OTG and related controls */
#define BQ25792_REG13_CHG_CTRL4     (0x13) /* REG13_Charger_Control_4 (8-bit) – Protection / comparator behavior controls */
#define BQ25792_REG14_CHG_CTRL5     (0x14) /* REG14_Charger_Control_5 (8-bit) – Misc charger options (device-specific) */
#define BQ25792_REG15_RESERVED      (0x15) /* REG15_Reserved (8-bit) – Reserved; keep default value */
#define BQ25792_REG16_TEMP_CTRL     (0x16) /* REG16_Temperature_Control (8-bit) – JEITA / temperature-based charge controls */
#define BQ25792_REG17_NTC_CTRL0     (0x17) /* REG17_NTC_Control_0 (8-bit) – NTC thresholds / TS function configuration */
#define BQ25792_REG18_NTC_CTRL1     (0x18) /* REG18_NTC_Control_1 (8-bit) – Additional NTC/TS configuration */
#define BQ25792_REG19_ICO_ILIM      (0x19) /* REG19_ICO_Current_Limit (16-bit) – ICO result / current limit setting (device-specific) */

#define BQ25792_REG1B_STATUS0       (0x1B) /* REG1B_Charger_Status_0 (8-bit) – Charger state / power-path status summary */
#define BQ25792_REG1C_STATUS1       (0x1C) /* REG1C_Charger_Status_1 (8-bit) – Input source / DPM / charge state indicators */
#define BQ25792_REG1D_STATUS2       (0x1D) /* REG1D_Charger_Status_2 (8-bit) – OTG/boost and additional status indicators */
#define BQ25792_REG1E_STATUS3       (0x1E) /* REG1E_Charger_Status_3 (8-bit) – NTC/thermal and related status indicators */
#define BQ25792_REG1F_STATUS4       (0x1F) /* REG1F_Charger_Status_4 (8-bit) – Misc status (timers, regulation, etc.) */

#define BQ25792_REG20_FAULT0        (0x20) /* REG20_Fault_Status_0 (8-bit) – Active fault indicators (power/voltage/current) */
#define BQ25792_REG21_FAULT1        (0x21) /* REG21_Fault_Status_1 (8-bit) – Active fault indicators (thermal/NTC/other) */

#define BQ25792_REG22_FLAG0         (0x22) /* REG22_Charger_Flag_0 (8-bit) – Charger event flags (interrupt sources) */
#define BQ25792_REG23_FLAG1         (0x23) /* REG23_Charger_Flag_1 (8-bit) – Charger event flags (interrupt sources) */
#define BQ25792_REG24_FLAG2         (0x24) /* REG24_Charger_Flag_2 (8-bit) – Charger event flags (interrupt sources) */
#define BQ25792_REG25_FLAG3         (0x25) /* REG25_Charger_Flag_3 (8-bit) – Charger event flags (interrupt sources) */

#define BQ25792_REG26_FAULT_FLAG0   (0x26) /* REG26_Latched_Fault_Flag_0 (8-bit) – Sticky fault flags (latched), cleared on read */
#define BQ25792_REG27_FAULT_FLAG1   (0x27) /* REG27_Latched_Fault_Flag_1 (8-bit) – Sticky fault flags (latched), cleared on read */

#define BQ25792_REG28_MASK0         (0x28) /* REG28_Charger_Interrupt_Mask_0 (8-bit) – Mask bits for FLAG0 sources */
#define BQ25792_REG29_MASK1         (0x29) /* REG29_Charger_Interrupt_Mask_1 (8-bit) – Mask bits for FLAG1 sources */
#define BQ25792_REG2A_MASK2         (0x2A) /* REG2A_Charger_Interrupt_Mask_2 (8-bit) – Mask bits for FLAG2 sources */
#define BQ25792_REG2B_MASK3         (0x2B) /* REG2B_Charger_Interrupt_Mask_3 (8-bit) – Mask bits for FLAG3 sources */
#define BQ25792_REG2C_FAULT_MASK0   (0x2C) /* REG2C_Fault_Interrupt_Mask_0 (8-bit) – Mask bits for FAULT_FLAG0 sources */
#define BQ25792_REG2D_FAULT_MASK1   (0x2D) /* REG2D_Fault_Interrupt_Mask_1 (8-bit) – Mask bits for FAULT_FLAG1 sources */

#define BQ25792_REG2E_ADC_CTRL      (0x2E) /* REG2E_ADC_Control (8-bit) – ADC enable/mode, sampling, averaging control */
#define BQ25792_REG2F_ADC_DIS0      (0x2F) /* REG2F_ADC_Function_Disable_0 (8-bit) – Disable ADC channels (group 0) */
#define BQ25792_REG30_ADC_DIS1      (0x30) /* REG30_ADC_Function_Disable_1 (8-bit) – Disable ADC channels (group 1) */

#define BQ25792_REG31_IBUS_ADC      (0x31) /* REG31_IBUS_ADC_Result (16-bit) – ADC result: input current (IBUS) */
#define BQ25792_REG33_IBAT_ADC      (0x33) /* REG33_IBAT_ADC_Result (16-bit) – ADC result: battery current (IBAT) */
#define BQ25792_REG35_VBUS_ADC      (0x35) /* REG35_VBUS_ADC_Result (16-bit) – ADC result: input voltage (VBUS) */
#define BQ25792_REG37_VAC1_ADC      (0x37) /* REG37_VAC1_ADC_Result (16-bit) – ADC result: VAC1 pin voltage */
#define BQ25792_REG39_VAC2_ADC      (0x39) /* REG39_VAC2_ADC_Result (16-bit) – ADC result: VAC2 pin voltage */
#define BQ25792_REG3B_VBAT_ADC      (0x3B) /* REG3B_VBAT_ADC_Result (16-bit) – ADC result: battery voltage (VBAT) */
#define BQ25792_REG3D_VSYS_ADC      (0x3D) /* REG3D_VSYS_ADC_Result (16-bit) – ADC result: system voltage (VSYS) */
#define BQ25792_REG3F_TS_ADC        (0x3F) /* REG3F_TS_ADC_Result (16-bit) – ADC result: TS/NTC voltage */
#define BQ25792_REG41_TDIE_ADC      (0x41) /* REG41_TDIE_ADC_Result (16-bit) – ADC result: die temperature */
#define BQ25792_REG43_DP_ADC        (0x43) /* REG43_DP_ADC_Result (16-bit) – ADC result: USB D+ voltage */
#define BQ25792_REG45_DM_ADC        (0x45) /* REG45_DM_ADC_Result (16-bit) – ADC result: USB D− voltage */

#define BQ25792_REG47_DPDM_DRIVER   (0x47) /* REG47_DPDM_Driver_Control (8-bit) – Controls DP/DM drive (BC1.2 / detection) */
#define BQ25792_REG48_PART_INFO     (0x48) /* REG48_Part_Information (8-bit) – Part number / revision / device ID */
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


/*------------------------ Typedefs and Enums ------------------------------------*/

typedef struct {
	I2C_HandleTypeDef *hi2c;
	GPIO_TypeDef *ce_port;
	uint16_t ce_pin;
}BQ25792_HandleTypeDef;
extern BQ25792_HandleTypeDef hbms;
typedef enum
{
    CHARGE_ENABLE,
    CHARGE_DISABLE
}BQ25792_CHARGE_STATE;

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
/* ----------------------- Function Definitions ---------------------- */

void bq25792_Init();
HAL_StatusTypeDef bq25792_read_u8(BQ25792_HandleTypeDef *hbms, uint8_t reg, uint8_t *val);
HAL_StatusTypeDef bq25792_write_u8(BQ25792_HandleTypeDef *hbms, uint8_t reg, uint8_t val);
HAL_StatusTypeDef bq25792_read_u16(BQ25792_HandleTypeDef *hbms, uint8_t reg, uint16_t *val);
HAL_StatusTypeDef bq25792_write_u16(BQ25792_HandleTypeDef *hbms, uint8_t reg, uint16_t val);
HAL_StatusTypeDef bq25792_update_bits_u8(BQ25792_HandleTypeDef *hbms, uint8_t reg, uint8_t mask, uint8_t value);
HAL_StatusTypeDef bq25792_probe(BQ25792_HandleTypeDef *hbms);
HAL_StatusTypeDef bq25792_set_charge_enable(BQ25792_HandleTypeDef *bms, BQ25792_CHARGE_STATE enable);
HAL_StatusTypeDef bq25792_quick_config(
    BQ25792_HandleTypeDef *hi2c,
    uint16_t vsysmin_mv,
    uint16_t vreg_mv,
    uint16_t ichg_ma,
    uint16_t vindpm_mv,
    uint16_t iindpm_ma
);
bool bq25792_reg_is_16bit(uint8_t reg);
#ifdef __cplusplus
}
#endif

#endif /* BQ25792_H */
