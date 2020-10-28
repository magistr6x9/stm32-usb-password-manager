#ifndef __BUTTON_H
#define __BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

typedef enum {
	BUTTON_STATUS_NONE,
	BUTTON_STATUS_WAITING_FOR_CONFIRMATION,		// ожидание подтверждения операции
	BUTTON_STATUS_CONFIRMED,			// операция подтверждена нажатием кнопки
	BUTTON_STATUS_TIMEOUT,				// кнопка в период подтверждения НЕ нажималась
	BUTTON_STATUS_PRESSED,				// если кнопка удерживается нажатой > 3сек - сброс и повторный запрос PIN
} buttonStatus_t;

void debounceButton(void);

extern volatile uint8_t button_down;
extern volatile uint8_t button_up;
extern buttonStatus_t button_status;

#ifdef __cplusplus
}
#endif

#endif
