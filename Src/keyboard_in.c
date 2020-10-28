#include "main.h"
#include "keyboard_in.h"
#include "password.h"

volatile uint8_t kbd_led_state = 255;

static volatile uint8_t _key_command_timer = 0;
static volatile uint8_t _key_press_count = 0;
static volatile uint8_t _key_press_count_prev = 0;
static volatile uint16_t _key_command_sequence = 0;

static volatile key_command_state_t _key_command_state = KEY_CMD_STATE_WAIT;

#define KEY_PRESS_TIMEOUT 50 // максимальная пауза между нажатиями клавиш == 50 переполнений == 1c

// вызывается при изменении состояния индикаторов CapsLock,NumLock,ScrollLock
void HID_KBD_GetReport(uint8_t *data)
{
  if ((kbd_led_state ^ data[0]) & CAPS_LOCK_MASK) // нажата CAPS_LOCK
  {
    _key_command_sequence <<= 2;
    _key_command_sequence |= CAPS_CODE;
    _key_press_count++;
  }
  else if ((kbd_led_state ^ data[0]) & NUM_LOCK_MASK) // нажата NUM_LOCK
  {
    _key_command_sequence <<= 2;
    _key_command_sequence |= NUM_CODE;
    _key_press_count++;
  }
  else if ((kbd_led_state ^ data[0]) & SCROLL_LOCK_MASK) // нажата SCROLL_LOCK
  {
    _key_command_sequence <<= 2;
    _key_command_sequence |= SCROLL_CODE;
    _key_press_count++;
  }

  kbd_led_state = data[0];
}

void waitKbdLedState(void)
{
  while (kbd_led_state == 255)
    ;
}

void keyCommandReset(void)
{
  _key_command_sequence = 0;
  _key_command_state = KEY_CMD_STATE_WAIT;
}

key_command_state_t keyCommandState(void)
{
  return _key_command_state;
}

uint16_t keyCommandSequence(void)
{
  return _key_command_sequence;
}

/* вызывается из обработчика прерывания */
void keyCommandTimerTask(void)
{

  // только если есть нажатые клавиши...
  if (_key_press_count > 0)
  {
    // если в режиме приема команд...
    if (_key_command_state == KEY_CMD_STATE_WAIT)
    {
      // если новых клавиш не нажимали...
      if (_key_press_count == _key_press_count_prev)
      {
        // время ожидания нажатия клавиши истекло ?
        if (++_key_command_timer > KEY_PRESS_TIMEOUT)
        {
          if (pin_code != 0)
          {
            _key_command_state = KEY_CMD_STATE_READY; // теперь можно в основном цикле распознать команду
          }
          else if (_key_press_count == 8)
          {
            _key_command_state = KEY_CMD_STATE_PIN;
          }

          // сброс счетчиков и таймера ожидания нажатия клавиши
          _key_press_count = 0;
          _key_press_count_prev = 0;
          _key_command_timer = 0;
        }
      }
      else
      {
        // в период ожидания была нажата клавиша, сбросить таймер и сохранить число нажатий
        _key_command_timer = 0;
        _key_press_count_prev = _key_press_count;
      }
    }
    else
      // нажатия клавиш не в режиме KEY_CMD_STATE_WAIT должны игнорироваться
      _key_press_count = 0;
  }
}
