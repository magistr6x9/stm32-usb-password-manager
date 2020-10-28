#ifndef __EEPROM_EMU_H
#define __EEPROM_EMU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void eepromErase(void);
int eepromWrite(uint32_t src, int offset, int size);
void eepromRead(uint32_t dst, int offset, int size);

#ifdef __cplusplus
}
#endif

#endif