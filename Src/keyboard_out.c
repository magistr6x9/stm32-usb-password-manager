#include "main.h"
#include "usb_device.h"
#include "usbd_hid.h"
#include "keyboard_out.h"

uint8_t USBD_HID_SendReport(USBD_HandleTypeDef *pdev,
                            uint8_t *report,
                            uint16_t len);

// ***********************************************************************
// * преобразование символа в код клавиши + положение модификатора SHIFT
// ***********************************************************************

void _charToKeyboardReport(uint8_t ch);

#define MOD_SHIFT_LEFT (1 << 1)
#define MOD_SHIFT_RIGHT (1 << 5)

uint8_t keyboard_report[8];

extern uint8_t kbd_led_state;
extern USBD_HandleTypeDef hUsbDeviceFS;

void typeMsg(uint8_t *m)
{
  register uint8_t c;

  while ((c = *m++) != '\0')
  {
    // заполняем структуру keyboard_report
    _charToKeyboardReport(c);

    // проверяем на присутствие нажатия правого или левого SHIFT
    // можно и просто сравнивать с нулем :)
    if ((keyboard_report[0] & (MOD_SHIFT_RIGHT | MOD_SHIFT_LEFT)) != 0)
    {
      // сохраняем код клавиши и сбрасываем байт символа в keyboard_report
      // имитируя сначала нажатие только SHIFT без клавиши
      c = keyboard_report[2];
      keyboard_report[2] = 0;

      while (USBD_HID_SendReport(
                 &hUsbDeviceFS, keyboard_report, sizeof(keyboard_report)) == USBD_BUSY)
        USBD_Delay(1);

      // восстанавливаем код клавиши и отправляем SHIFT + клавиша
      keyboard_report[2] = c;
      while (USBD_HID_SendReport(
                 &hUsbDeviceFS, keyboard_report, sizeof(keyboard_report)) == USBD_BUSY)
        USBD_Delay(1);

      // снова сбрасываем код клавиши и отправляем только SHIFT, имитируя отпускание клавиши
      keyboard_report[2] = 0;
    }

    while (USBD_HID_SendReport(
               &hUsbDeviceFS, keyboard_report, sizeof(keyboard_report)) == USBD_BUSY)
      USBD_Delay(1);

    // в конце все клавиши отпущены
    clearReport();
    while (USBD_HID_SendReport(
               &hUsbDeviceFS, keyboard_report, sizeof(keyboard_report)) == USBD_BUSY)
      USBD_Delay(1);
  }

  clearReport();
}

void _charToKeyboardReport(uint8_t ch)
{

  if (ch >= '0' && ch <= '9')
  {
    keyboard_report[0] = 0;
    keyboard_report[2] = (ch == '0') ? 39 : 30 + (ch - '1');
  }
  else if (ch >= 'a' && ch <= 'z')
  {
    keyboard_report[0] = (kbd_led_state & 2) ? MOD_SHIFT_RIGHT : 0;
    keyboard_report[2] = 4 + (ch - 'a');
  }
  else if (ch >= 'A' && ch <= 'Z')
  {
    keyboard_report[0] = (kbd_led_state & 2) ? 0 : MOD_SHIFT_RIGHT;
    keyboard_report[2] = 4 + (ch - 'A');
  }
  else
  {
    keyboard_report[0] = 0;
    keyboard_report[2] = 0;

    switch (ch)
    {
    // заимствовано из кода USB PCB Business Card http://www.frank-zhao.com/card/
    case '!':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      keyboard_report[2] = 29 + 1;
      break;
    case '@':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      keyboard_report[2] = 29 + 2;
      break;
    case '#':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      keyboard_report[2] = 29 + 3;
      break;
    case '$':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      keyboard_report[2] = 29 + 4;
      break;
    case '%':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      keyboard_report[2] = 29 + 5;
      break;
    case '^':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      keyboard_report[2] = 29 + 6;
      break;
    case '&':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      keyboard_report[2] = 29 + 7;
      break;
    case '*':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      keyboard_report[2] = 29 + 8;
      break;
    case '(':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      keyboard_report[2] = 29 + 9;
      break;
    case ')':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      keyboard_report[2] = 0x27;
      break;
    case '~':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      // fall through
    case '`':
      keyboard_report[2] = 0x35;
      break;
    case '_':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      // fall through
    case '-':
      keyboard_report[2] = 0x2D;
      break;
    case '+':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      // fall through
    case '=':
      keyboard_report[2] = 0x2E;
      break;
    case '{':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      // fall through
    case '[':
      keyboard_report[2] = 0x2F;
      break;
    case '}':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      // fall through
    case ']':
      keyboard_report[2] = 0x30;
      break;
    case '|':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      // fall through
    case '\\':
      keyboard_report[2] = 0x31;
      break;
    case ':':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      // fall through
    case ';':
      keyboard_report[2] = 0x33;
      break;
    case '"':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      // fall through
    case '\'':
      keyboard_report[2] = 0x34;
      break;
    case '<':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      // fall through
    case ',':
      keyboard_report[2] = 0x36;
      break;
    case '>':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      // fall through
    case '.':
      keyboard_report[2] = 0x37;
      break;
    case '?':
      keyboard_report[0] = MOD_SHIFT_RIGHT;
      // fall through
    case '/':
      keyboard_report[2] = 0x38;
      break;
    case ' ':
      keyboard_report[2] = 0x2C;
      break;
    case '\t':
      keyboard_report[2] = 0x2B;
      break;
    case '\n':
      keyboard_report[2] = 0x28;
      break;
    }
  }
}
