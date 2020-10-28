#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define clearReport() {keyboard_report[0] = keyboard_report[2] = 0;}

extern uint8_t keyboard_report[8];

void typeMsg(uint8_t *message);

#ifdef __cplusplus
}
#endif

#endif
