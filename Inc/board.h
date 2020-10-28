#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#define _MAPLE_MINI_BOARD
// #define _BLUE_PILL_BOARD

/* Maple Mini */
#ifdef _MAPLE_MINI_BOARD

#define USER_LED_Pin GPIO_PIN_1
#define USER_LED_GPIO_Port GPIOB
#define USER_LED_PIN_On GPIO_PIN_SET
#define USER_LED_PIN_Off GPIO_PIN_RESET
#define USER_BUTTON_Pin GPIO_PIN_8
#define USER_BUTTON_GPIO_Port GPIOB
#define USER_BUTTON_PIN_Pressed GPIO_PIN_SET
#define USER_BUTTON_GPIO_Pull GPIO_NOPULL
#define USB_DISC_Pin GPIO_PIN_9
#define USB_DISC_GPIO_Port GPIOB

#endif

/* Blue pill */
#ifdef _BLUE_PILL_BOARD

#define USER_LED_Pin GPIO_PIN_13
#define USER_LED_GPIO_Port GPIOC
#define USER_LED_PIN_On GPIO_PIN_RESET
#define USER_LED_PIN_Off GPIO_PIN_SET
#define USER_BUTTON_Pin GPIO_PIN_14
#define USER_BUTTON_GPIO_Port GPIOA
#define USER_BUTTON_PIN_Pressed GPIO_PIN_RESET
#define USER_BUTTON_GPIO_Pull GPIO_PULLUP

#endif

#ifdef __cplusplus
}
#endif

#endif