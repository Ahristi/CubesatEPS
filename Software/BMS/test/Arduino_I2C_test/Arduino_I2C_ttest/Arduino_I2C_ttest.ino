#include <Wire.h>

#define BMS_ADDR 0x6B

#define BMS_REG31_I_SYS     0x31
#define BMS_REG33_I_BATT    0x33
#define BMS_REG35_V_UMB     0x35
#define BMS_REG37_V_PANEL0  0x37
#define BMS_REG39_V_PANEL1  0x39
#define BMS_REG3B_V_BATT    0x3B
#define BMS_REG3D_V_SYS     0x3D
#define BMS_REG3F_T_BATT    0x3F
#define BMS_REG41_DIE_TEMP  0x41
#define BMS_REG43_I_PANEL0  0x43
#define BMS_REG45_I_PANEL1  0x45

uint16_t readReg16(uint8_t reg)
{
    Wire.beginTransmission(BMS_ADDR);
    Wire.write(reg);
    uint8_t err = Wire.endTransmission(false); // false = repeated start

    if (err != 0)
    {
        Serial.print("I2C write error: ");
        Serial.println(err);
        return 0xFFFF;
    }

    uint8_t n = Wire.requestFrom((uint8_t)BMS_ADDR, (uint8_t)2);

    if (n != 2)
    {
        Serial.print("I2C read error, bytes received: ");
        Serial.println(n);
        return 0xFFFF;
    }

    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();

    return ((uint16_t)msb << 8) | lsb;
}

void printReg(const char *name, uint8_t reg)
{
    uint16_t val = readReg16(reg);

    Serial.print(name);
    Serial.print(" 0x");
    Serial.print(reg, HEX);
    Serial.print(" = ");
    Serial.print(val);
    Serial.print(" / 0x");
    Serial.println(val, HEX);
}

void setup()
{
    Serial.begin(115200);
    Wire.begin();

    delay(1000);

    Serial.println("STM32 BMS I2C slave test");
}

void loop()
{
    printReg("I_SYS    ", BMS_REG31_I_SYS);
    printReg("I_BATT   ", BMS_REG33_I_BATT);
    printReg("V_UMB    ", BMS_REG35_V_UMB);
    printReg("V_PANEL0 ", BMS_REG37_V_PANEL0);
    printReg("V_PANEL1 ", BMS_REG39_V_PANEL1);
    printReg("V_BATT   ", BMS_REG3B_V_BATT);
    printReg("V_SYS    ", BMS_REG3D_V_SYS);
    printReg("T_BATT   ", BMS_REG3F_T_BATT);
    printReg("DIE_TEMP ", BMS_REG41_DIE_TEMP);
    printReg("I_PANEL0 ", BMS_REG43_I_PANEL0);
    printReg("I_PANEL1 ", BMS_REG45_I_PANEL1);

    Serial.println();

    delay(1000);
}