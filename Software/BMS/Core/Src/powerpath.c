#include "powerpath.h"

POWERPATH_Handler_t hpowerpath;




void POWERPATH_Init(void)
{
    POWERPATH_setState(STATE_STARTUP);
    POWERPATH_enableBattery();
    POWERPATH_enableUmbilical();
    POWERPATH_disableCharger();
    POWERPATH_disableChargereFuse();
}


void POWERPATH_Task(void)
{
    switch (hpowerpath.state)
    {
    case STATE_STARTUP:
    {
        POWERPATH_determineState();
        break;
    }
    case STATE_MPPT_DISABLE:
    {
        if (hpowerpath.entering_state)
        {
            hpowerpath.entering_state = false;
            MPPT_disable(&hmppt0);
            MPPT_disable(&hmppt1);
        }
        else //TODO: Add some kind of check before entering efuse enable state
        {
            POWERPATH_setState(STATE_CHARGE_EFUSE_ENABLE);
        }
        break;
    }
    case STATE_CHARGE_EFUSE_ENABLE:
    {
        if (hpowerpath.entering_state)
        {
            hpowerpath.entering_state = false;
            POWERPATH_enableChargereFuse();
        }
        else if (HAL_GPIO_ReadPin(CHG_PG_GPIO_Port, CHG_PG_Pin) == GPIO_PIN_SET)
        {
            POWERPATH_setState(STATE_CHARGER_ENABLE);
        }
        else if (POWERPATH_stateTimedOut(POWERSTAGE_TIMEOUT_MS))
        {
            hpowerpath.faults |= FAULT_MASK_CHG_EFUSE_TIMEOUT;
            POWERPATH_setState(STATE_FAULT);
        }
        break;
    }
    case STATE_CHARGER_ENABLE:
    {
        if (hpowerpath.entering_state)
        {
            hpowerpath.entering_state = false;
            POWERPATH_enableCharger();
        }
        else if (HAL_GPIO_ReadPin(nCHG_OK_GPIO_Port, nCHG_OK_Pin) == GPIO_PIN_RESET)
        {
            POWERPATH_setState(STATE_UMBILICAL_CHARGING);
        }
        else if (POWERPATH_stateTimedOut(POWERSTAGE_TIMEOUT_MS))
        {
            hpowerpath.faults |= FAULT_MASK_CHARGER_ENABLE_TIMEOUT;
            POWERPATH_setState(STATE_FAULT);
        }
        break;
    }
    case STATE_UMBILICAL_CHARGING:
    {
        if (hpowerpath.entering_state)
        {
            hpowerpath.entering_state = false;
        }
        else if (HAL_GPIO_ReadPin(UMB_PG_GPIO_Port, UMB_PG_Pin) == GPIO_PIN_RESET)
        {
            POWERPATH_setState(STATE_CHARGER_DISABLE);
        }
        break;
    }
    case STATE_CHARGER_DISABLE:
    {
        if (hpowerpath.entering_state)
        {
            hpowerpath.entering_state = false;
            POWERPATH_disableCharger();
        }
        else if (HAL_GPIO_ReadPin(nCHG_OK_GPIO_Port, nCHG_OK_Pin) == GPIO_PIN_SET)
        {
            POWERPATH_setState(STATE_CHARGER_EFUSE_DISABLE);
        }
        else if (POWERPATH_stateTimedOut(POWERSTAGE_TIMEOUT_MS))
        {
            hpowerpath.faults |= FAULT_MASK_CHARGER_DISABLE_TIMEOUT;
            POWERPATH_setState(STATE_FAULT);
        }
        break;
    }
    case STATE_CHARGER_EFUSE_DISABLE:
    {
        if (hpowerpath.entering_state)
        {
            hpowerpath.entering_state = false;
            POWERPATH_disableChargereFuse();
        }
        else if (HAL_GPIO_ReadPin(CHG_PG_GPIO_Port, CHG_PG_Pin) == GPIO_PIN_RESET)
        {
            POWERPATH_setState(STATE_MPPT_ENABLE);
        }
        else if (POWERPATH_stateTimedOut(POWERSTAGE_TIMEOUT_MS))
        {
            hpowerpath.faults |= FAULT_MASK_CHG_EFUSE_DISABLE_TIMEOUT;
            POWERPATH_setState(STATE_FAULT);
        }
        break;
    }
    case STATE_MPPT_ENABLE:
    {
        if (hpowerpath.entering_state)
        {
            hpowerpath.entering_state = false;
            if (!MPPT_enable(&hmppt0))
            {
                hpowerpath.faults |= FAULT_MASK_PANEL0_MISSING;
            }
            if (!MPPT_enable(&hmppt1))
            {
                hpowerpath.faults |= FAULT_MASK_PANEL1_MISSING;
            }
        }
        else 
        {
            POWERPATH_setState(STATE_NORMAL);
        }
        break;
    }
    case STATE_NORMAL:
    {
        if (hpowerpath.entering_state)
        {
            hpowerpath.entering_state = false;
        }
        else if (HAL_GPIO_ReadPin(UMB_PG_GPIO_Port, UMB_PG_Pin) == GPIO_PIN_SET)
        {
            POWERPATH_setState(STATE_MPPT_DISABLE);
        }
        break;
    }
    case STATE_FAULT:
    {
        if (hpowerpath.entering_state)
        {
            hpowerpath.entering_state = false;

            POWERPATH_disableCharger();
            POWERPATH_disableChargereFuse();

            MPPT_disable(&hmppt0);
            MPPT_disable(&hmppt1);
        }
        break;
    }
    default:
    {
        hpowerpath.faults |= FAULT_MASK_INVALID_STATE;
        POWERPATH_setState(STATE_FAULT);
        break;
    }
        break;
    }
}


void POWERPATH_determineState(void)
{
    if(HAL_GPIO_ReadPin(UMB_PG_GPIO_Port, UMB_PG_Pin) == GPIO_PIN_SET)
    {
        POWERPATH_setState(STATE_MPPT_DISABLE); //Begin transition to charge from umbilical
        return;
    }
    else if (HAL_GPIO_ReadPin(BAT_PG_GPIO_Port, BAT_PG_Pin) == GPIO_PIN_SET)
    {
        POWERPATH_setState(STATE_CHARGER_DISABLE); //Begin transition to charge from solar panels
        return; 
    }
    else
    {
        hpowerpath.faults |= FAULT_MASK_INVALID_STATE;
        POWERPATH_setState(STATE_FAULT); //Unknown state
    }
}


void POWERPATH_setState(POWERPATH_State_t new_state)
{
    hpowerpath.state = new_state;
    hpowerpath.entering_state = true;
    hpowerpath.state_entry_ms = HAL_GetTick();
}


bool POWERPATH_stateTimedOut(uint32_t timeout_ms)
{
    uint32_t now = HAL_GetTick();

    return (now - hpowerpath.state_entry_ms) >= timeout_ms;
}


void POWERPATH_enableCharger(void)
{
    HAL_GPIO_WritePin(nCHG_EN_GPIO_Port, nCHG_EN_Pin, GPIO_PIN_RESET);
}

void POWERPATH_disableCharger(void)
{
    HAL_GPIO_WritePin(nCHG_EN_GPIO_Port, nCHG_EN_Pin, GPIO_PIN_SET);
}

void POWERPATH_enableChargereFuse(void)
{
    HAL_GPIO_WritePin(CHG_SHDN_GPIO_Port, CHG_SHDN_Pin, GPIO_PIN_RESET);
}

void POWERPATH_disableChargereFuse(void)
{
    HAL_GPIO_WritePin(CHG_SHDN_GPIO_Port, CHG_SHDN_Pin, GPIO_PIN_SET);
}

void POWERPATH_enableUmbilical(void)
{
    HAL_GPIO_WritePin(UMB_SHDN_GPIO_Port, UMB_SHDN_Pin, GPIO_PIN_RESET);

}
void POWERPATH_disableUmbilical(void)
{
    HAL_GPIO_WritePin(UMB_SHDN_GPIO_Port, UMB_SHDN_Pin, GPIO_PIN_SET);
}

void POWERPATH_enableBattery(void)
{
    HAL_GPIO_WritePin(BAT_SHDN_GPIO_Port, BAT_SHDN_Pin, GPIO_PIN_RESET);
}

void POWERPATH_disableBattery(void)
{
    HAL_GPIO_WritePin(BAT_SHDN_GPIO_Port, BAT_SHDN_Pin, GPIO_PIN_SET);   
}



