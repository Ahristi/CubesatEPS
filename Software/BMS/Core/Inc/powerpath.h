#ifndef __POWERPATH_H__
#define __POWERPATH_H__
#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "mppt.h"

//-------------Defines-------------
#define FAULT_MASK_MISSING_BATTERY                  1U << 0
#define FAULT_MASK_PANEL0_MISSING                   1U << 1
#define FAULT_MASK_PANEL1_MISSING                   1U << 2
#define FAULT_MASK_CHG_EFUSE_TIMEOUT                1U << 3
#define FAULT_MASK_CHARGER_ENABLE_TIMEOUT           1U << 4  
#define FAULT_MASK_CHARGER_DISABLE_TIMEOUT          1U << 5    
#define FAULT_MASK_CHG_EFUSE_DISABLE_TIMEOUT        1U << 6      
#define FAULT_MASK_INVALID_STATE                    1U << 7  



#define POWERSTAGE_TIMEOUT_MS 500

//-------------Typedef and Enums-------------
typedef enum{
    STATE_STARTUP,              //Determine power source and configure charger accordingly.
    
    STATE_MPPT_DISABLE,         //Umbilical plugged in. Disable MPPTs before charging.
    STATE_CHARGE_EFUSE_ENABLE,  //Start by enabling the charger input eFuse
    STATE_CHARGER_ENABLE,       //Once the MPPTs have been successfully disabled and charger eFuse has been enabled, enable the charger.
    STATE_UMBILICAL_CHARGING,   //Powered from umbilical. Battery disconnected from system and being charged.
    STATE_UMBILICAL_NO_CHARGE,  //Powered from umbilical. Battery disconnected from system. Charger not active.


    STATE_CHARGER_DISABLE,      //Umbilical removed. Disable charger IC.
    STATE_CHARGER_EFUSE_DISABLE,//Once charger has been turned off, turn off the charger input eFuse.
    STATE_MPPT_ENABLE,          //After charger power down sequence, enable the MPPT outputs
    STATE_NORMAL,               //Powered from battery. No Umbilical, MPPTs active

    STATE_FAULT                 //Fault during power staging
}POWERPATH_State_t;


typedef struct {
    POWERPATH_State_t state;
    bool entering_state;
    uint32_t state_entry_ms;
    
    uint16_t faults;
}POWERPATH_Handler_t;
extern POWERPATH_Handler_t hpowerpath;


//-------------Function Prototypes-------------
void POWERPATH_Init(void);
void POWERPATH_Task(void);
void POWERPATH_determineState(void);
void POWERPATH_enableCharger(void);
void POWERPATH_disableCharger(void);
void POWERPATH_enableChargereFuse(void);
void POWERPATH_disableChargereFuse(void);
void POWERPATH_enableMPPT(void);
void POWERPATH_disableMPPT(void);
void POWERPATH_enableUmbilical(void);
void POWERPATH_disableUmbilical(void);
void POWERPATH_enableBattery(void);
void POWERPATH_disableBattery(void);

void POWERPATH_setState(POWERPATH_State_t new_state);
bool POWERPATH_stateTimedOut(uint32_t timeout_ms);

#endif