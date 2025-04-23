#ifndef _MODBUS_EEPROM_SL_H_
#define _MODBUS_EEPROM_SL_H_

#include "../Hardware_Interface_EEPROM/Hardware_Interface_EEPROM.h"
#include "../HMI_Modbus/HMI_Modbus.h"

extern AG_HW_Interface_t HWIN;

#define Write 0
#define Read 1

#define Size 						(sizeof(SystemParams_t))
#define Size_Default				(sizeof(DefaultParams))
#define StartAfterCheckSum	4


/// @fn uint8_t Load_NV_Data()
/// @brief
/// Loads the NV data from the EEPROM
/// @return 1 for success, and 0 for failure
uint8_t Load_NV_Data();

/// @fn uint8_t Load_Default_NV_Data()
/// @brief
/// Loads the hard coded default values into the EEPROM
/// @return 1 for success
uint8_t Load_Default_NV_Data();

/// @fn uint8_t Save_NV_Data()
/// @brief
/// Saves the NV data into the EEPROM
/// @return 1 for success, and 0 for otherwise
uint8_t Save_NV_Data();
#endif /*_MODBUS_EEPROM_SL_H_*/
