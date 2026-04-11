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

/* REG0F Charger Control 0 (Offset 0x0F) Settings*/
#define BQ25792_REG0F_EN_AUTO_IBATDIS         BQ25792_BIT(7)
#define BQ25792_REG0F_FORCE_IBATDIS           BQ25792_BIT(6)
#define BQ25792_REG0F_EN_CHG                  BQ25792_BIT(5)
#define BQ25792_REG0F_EN_ICO                  BQ25792_BIT(4)
#define BQ25792_REG0F_FORCE_ICO               BQ25792_BIT(3)
#define BQ25792_REG0F_EN_HIZ                  BQ25792_BIT(2)
#define BQ25792_REG0F_EN_TERM                 BQ25792_BIT(1)
/* bit0 reserved */

/* REG0F Charger Control 1 (Offset 0x10) Settings*/
/* bit7-6 reserved */
#define BQ25792_REG10_VAC_OVP                 (0x00 <<4)     //26V OVP
#define BQ25792_REG10_WD_RST                  BQ25792_BIT(3) //Reset watchdog
#define BQ25792_REG10_WATCHDOG                0x05;          //Watchdog set to 50s


/* REG0F Charger Control 5 (Offset 0x14) Settings*/
#define BQ25792_REG14_SFET_PRESENT            !BQ25792_BIT(7)
/* bit6 reserved */
#define BQ25792_REG14_EN_IBAT                  BQ25792_BIT(5)
#define BQ25792_REG14_IBAT_REG                (0x00 <<3)
#define BQ25792_REG14_EN_IINDPM                BQ25792_BIT(2)
#define BQ25792_REG14_EN_EXTILIM               BQ25792_BIT(1)
#define BQ25792_REG14_EN_BATOC                !BQ25792_BIT(1)

/* REG2E ADC Control (Offset 0x2E)*/
#define BQ25792_REG2E_ADC_EN                  BQ25792_BIT(7)
#define BQ25792_REG2E_ADC_RATE                BQ25792_BIT(6)          /* 0=continuous, 1=one-shot */
#define BQ25792_REG2E_ADC_SAMPLE_MASK         (0x3u << 4)             /* bits[5:4] */
#define BQ25792_REG2E_ADC_AVG                 BQ25792_BIT(3)
#define BQ25792_REG2E_ADC_AVG_INIT            BQ25792_BIT(2)
/* bits[1:0] reserved */

/* REG2F ADC Function Disable 0 (Offset 0x2F)*/
#define BQ25792_REG2F_IBUS_ADC_DIS            BQ25792_BIT(7)
#define BQ25792_REG2F_IBAT_ADC_DIS            BQ25792_BIT(6)
#define BQ25792_REG2F_VBUS_ADC_DIS            BQ25792_BIT(5)
#define BQ25792_REG2F_VBAT_ADC_DIS            BQ25792_BIT(4)
#define BQ25792_REG2F_VSYS_ADC_DIS            BQ25792_BIT(3)
#define BQ25792_REG2F_TS_ADC_DIS              BQ25792_BIT(2)
#define BQ25792_REG2F_TDIE_ADC_DIS            BQ25792_BIT(1)
/* bit0 reserved */

/* REG30 ADC Function Disable 1 (Offset 0x30)*/
#define BQ25792_REG30_DP_ADC_DIS              BQ25792_BIT(7)
#define BQ25792_REG30_DM_ADC_DIS              BQ25792_BIT(6)
#define BQ25792_REG30_VAC2_ADC_DIS            BQ25792_BIT(5)
#define BQ25792_REG30_VAC1_ADC_DIS            BQ25792_BIT(4)
/* bits[3:0] reserved */



/* ----------------------- Constants and Conversion ----------------------- */
#define REG3F_TS_ADC_BIT_STEP 0.0976563/100.0f
#define VREGN 4.8
#define R11 9310.0f
#define R12 330000.0f
#define THERM_R0     10000.0f   // 10kΩ at 25°C
#define THERM_T0     298.15f     // 25°C in Kelvin
#define THERM_BETA   3435.0f     // Beta value

/* ----------------------- Typedefs and Enums ----------------------- */

typedef struct {
	I2C_HandleTypeDef* hi2c;
	//Important analog measurements


	uint16_t rawBatteryVoltage;
	float batteryVoltage;

	uint16_t rawBatteryCurrent;
	float batteryCurrent;

	int16_t rawBatteryTemp;
	float batteryTemp;

	int16_t rawDieTemp;
	float dieTemp;

	uint16_t rawBusCurrent;
	float busCurrent;


	uint16_t rawBusVoltage;
	float busVoltage;

	uint16_t rawAC1Voltage;
	float AC1Voltage;

	uint16_t rawAC2Voltage;
	float AC2Voltage;

	uint16_t rawSysVoltage;
	float sysVoltage;

	//Status/Fault registers
	uint8_t chargerFlag0;
	uint8_t chargerFlag1;
	uint8_t chargerFlag2;
	uint8_t chargerFlag3;
	uint8_t faultFlag0;
	uint8_t faultFlag1;
}BMS_handlerTypedef;



/* ----------------------- Function Prototypes ----------------------- */
void BMS_init(I2C_HandleTypeDef* hi2c);
void BMS_read_u8(BMS_handlerTypedef* hbms, uint8_t reg, uint8_t* val);
void BMS_write_u8(BMS_handlerTypedef* hbms, uint8_t reg, uint8_t val);
void BMS_read_u16(BMS_handlerTypedef* hbms, uint8_t reg, uint16_t* val);
void BMS_write_u16(BMS_handlerTypedef* hbms, uint8_t reg, uint16_t val);
void BMS_getBatteryVoltage();
void BMS_getBatteryTemperature();
void BMS_getDieTemperature();
void BMS_getBusCurrent();
void BMS_getBatteryCurrent();
void BMS_getBusVoltage();
void BMS_getSysVoltage();

void BMS_resetWatchdog();

void BMS_configureADCs();
void BMS_readBMS();
float BMS_convertBatteryTemp(uint16_t rawTemp);
float BMS_convertDieTemp(uint16_t rawTemp);
float BMS_convertBusCurrent(uint16_t rawCurrent);
float BMS_convertBatteryCurrent(uint16_t rawCurrent);
float BMS_convertBusVoltage(uint16_t rawBusVoltage);
float BMS_convertSysVoltage(uint16_t rawVoltage);
extern BMS_handlerTypedef hbms;


#endif /* INC_BMS_H_ */
