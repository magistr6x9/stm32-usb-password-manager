#ifndef __KEYBOARD_CMD_H
#define __KEYBOARD_CMD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

// ***************
// * коды команд
// ***************

#define KEY_COMMAND_TYPEPASS_CAPS 0b10101010         // CAPS x 4
#define KEY_COMMAND_TYPEPASS_NUM 0b01010101          // NUM x 4
#define KEY_COMMAND_TYPEPASS_SCROLL 0b11111111       // SCROLL x 4
#define KEY_COMMAND_GENPASS 0b101101101101           // CSNCSN
#define KEY_COMMAND_GENPASS_12 0b100111100111        // CNSCNS
#define KEY_COMMAND_GENPASS_15 0b111001111001        // SCNSCN
#define KEY_COMMAND_TYPENEWPASS 0b11011101           // SNSN
#define KEY_COMMAND_STORPASS_CAPS 0b110111011010     // SNSNCC
#define KEY_COMMAND_STORPASS_NUM 0b110111010101      // SNSNNN
#define KEY_COMMAND_STORPASS_SCROLL 0b110111011111   // SNSNSS
#define KEY_COMMAND_PRINT_HISTORY 0b1011010110110101 // CSNNCSNN
#define KEY_COMMAND_CLEAR_HISTORY 0b1001111110011111 // CNSSCNSS

#define NUM_LOCK_MASK 0x01
#define CAPS_LOCK_MASK 0x02
#define SCROLL_LOCK_MASK 0x04

#define NUM_CODE 0b01
#define CAPS_CODE 0b10
#define SCROLL_CODE 0b11

// ***********************************
// * состояния интерпретатора команд
// ***********************************

typedef enum
{
  KEY_CMD_STATE_WAIT,  // готовы принять код клавиши
  KEY_CMD_STATE_READY, // коды клавиш переданы для декодирования
  KEY_CMD_STATE_PIN    // принят пин-код
} key_command_state_t;

void HID_KBD_GetReport(uint8_t *data);
void waitKbdLedState(void);
void keyCommandReset(void);
key_command_state_t keyCommandState(void);
uint16_t keyCommandSequence(void);
void keyCommandTimerTask(void);

extern uint8_t pin_code;
extern volatile uint8_t kbd_led_state;
extern volatile uint8_t simple_delay_counter;

#ifdef __cplusplus
}
#endif

#endif
