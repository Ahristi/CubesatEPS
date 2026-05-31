#include "telemetry.h"


uint8_t bms_register_map[REGISTER_MAP_SIZE];
static volatile uint16_t current_reg = 0;
static uint8_t rx_byte;
static uint8_t tx_buf[2];


void TELEMETRY_Init(void)
{
    HAL_I2C_EnableListen_IT(&hi2c3);
}

void TELEMETRY_Task(void)
{
    /*
        Note: This doesn't result in the same conversion values as the BQ25792 so its not
              "truly" directly swappable with the old BMS. I can't be fucked to do the conversions, 
              and we need to change the PDM software anyway to include the solar panel measurements.
    */


    TELEMETRY_WriteRegU16(BMS_REG31_I_SYS,    measurements[ADC_SYS_IMON].raw);
    TELEMETRY_WriteRegU16(BMS_REG33_I_BATT,   measurements[ADC_BATT_IMON].raw);
    TELEMETRY_WriteRegU16(BMS_REG35_V_UMB,    measurements[ADC_UMB_VMON].raw);
    TELEMETRY_WriteRegU16(BMS_REG37_V_PANEL0, measurements[ADC_PANEL0_VMON].raw);
    TELEMETRY_WriteRegU16(BMS_REG39_V_PANEL1, measurements[ADC_PANEL1_VMON].raw);
    TELEMETRY_WriteRegU16(BMS_REG3B_V_BATT,   measurements[ADC_BATT_VMON].raw);
    TELEMETRY_WriteRegU16(BMS_REG3D_V_SYS,    measurements[ADC_SYS_VMON].raw);
    TELEMETRY_WriteRegU16(BMS_REG3F_T_BATT,   measurements[ADC_BAT_TMON].raw);
    TELEMETRY_WriteRegU16(BMS_REG41_DIE_TEMP, 0); //TODO: Implement die temp adc
    TELEMETRY_WriteRegU16(BMS_REG43_I_PANEL0, measurements[ADC_PANEL0_IMON].raw);
    TELEMETRY_WriteRegU16(BMS_REG45_I_PANEL1, measurements[ADC_PANEL1_IMON].raw);
}

void TELEMETRY_WriteRegU8(uint16_t addr, uint8_t val)
{
    if (addr < REGISTER_MAP_SIZE)
    {
        bms_register_map[addr] = val;
    }
}

void TELEMETRY_WriteRegU16(uint16_t addr, uint16_t val)
{
    if ((addr + 1) < REGISTER_MAP_SIZE)
    {
        bms_register_map[addr]     = val >> 8;
        bms_register_map[addr + 1] = val & 0xFF;
    }
}

uint8_t TELEMETRY_ReadRegU8(uint16_t addr)
{
    if (addr < REGISTER_MAP_SIZE)
    {
        return bms_register_map[addr];
    }

    return 0;
}

uint16_t TELEMETRY_ReadRegU16(uint16_t addr)
{
    if ((addr + 1) < REGISTER_MAP_SIZE)
    {
        return ((uint16_t)bms_register_map[addr] << 8) | bms_register_map[addr + 1];
    }

    return 0;
}



//----------------I2C Interrupts: Note this logic was AI generated and not my work----------------
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
    if (hi2c != &hi2c3)
        return;

    if (TransferDirection == I2C_DIRECTION_TRANSMIT) 
    {
        /*
         * Master is writing to us.
         * First byte is interpreted as register address.
         */
        HAL_I2C_Slave_Sequential_Receive_IT(
            hi2c,
            &rx_byte,
            1,
            I2C_FIRST_AND_LAST_FRAME
        );
    }
    else
    {
        /*
         * Master is reading from us.
         * Return two bytes from current register.
         */
        if (current_reg + 1 < REGISTER_MAP_SIZE)
        {
            tx_buf[0] = bms_register_map[current_reg];
            tx_buf[1] = bms_register_map[current_reg + 1];
        }
        else
        {
            tx_buf[0] = 0;
            tx_buf[1] = 0;
        }

        HAL_I2C_Slave_Sequential_Transmit_IT(
            hi2c,
            tx_buf,
            2,
            I2C_FIRST_AND_LAST_FRAME
        );
    }
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c != &hi2c3)
        return;

    current_reg = rx_byte;

    if (current_reg >= REGISTER_MAP_SIZE)
    {
        current_reg = 0;
    }
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c != &hi2c3)
        return;

    HAL_I2C_EnableListen_IT(hi2c);
}
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c != &hi2c3)
        return;

    HAL_I2C_EnableListen_IT(hi2c);
}