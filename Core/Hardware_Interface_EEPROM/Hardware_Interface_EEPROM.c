/*
 * Hardware_Interface.c
 *
 *  Created on: Nov 27, 2024
 *      Author: M.W
 */

#include "Hardware_Interface_EEPROM.h"

AG_HW_Interface_t HWIN = { .EE_Commit_HWIN = EE_Commit_DI, .EE_Read_HWIN =
		EE_Read_DI, .EE_Write2ram_HWIN = EE_Write2ram_DI, .EE_Write_HWIN =
		EE_Write_DI };

void EE_Write_DI(uint32_t Add, uint32_t Len, uint8_t *Value) {
	ee_write(Add, Len, Value);
}

void EE_Read_DI(uint32_t Add, uint32_t Len, uint8_t *Value) {
	ee_read(Add, Len, Value);
}

void EE_Write2ram_DI(uint32_t Add, uint32_t Len, uint8_t *Data) {
	ee_writeToRam(Add, Len, Data);
}

void EE_Commit_DI() {
	ee_commit();
}



