#include "string.h"
#include "main.h"
#include "keyboard_out.h"
#include "password.h"
#include "eeprom_emu.h"

// копия паролей из EEPROM
uint8_t stored_password[3 + PASSWORD_HISTORY_LENGTH][PASSWORD_MAX_LENGTH];

// индекс следующей ячейки для записи "истории"
uint32_t password_history_index;

// переменные для работы утилиты командной строки
uint8_t local_password_copy[PASSWORD_MAX_LENGTH];

// сгенерированный пароль
uint8_t new_password[PASSWORD_MAX_LENGTH] = "";

// ***********
// * пин-код
// ***********

uint8_t pin_code = 0;

// ************************************************************************
// * функция заполняет new_password[], параметр len - длина нового пароля
// ************************************************************************

// использовать простой алгоритм псевдослучайных чисел + XOR со случайным числом
static uint8_t lfsr_xor_value;

void initRandomGenerator(void)
{
  lfsr_xor_value = HAL_GetTick() & 0xff;
}

void generatePassword(uint8_t len)
{
  uint8_t cindex, pindex, special, small, capital, digit, ch, prev_cindex = 255;
  static unsigned long lfsr = 1;

  do
  {
    small = capital = digit = special = 0;

    for (pindex = 0; pindex < len; ++pindex)
    {

      // "случайный" символ в диапазоне от 0 до 71
      do
      {
        lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xd0000001u);
        cindex = (unsigned char)lfsr;
        cindex ^= lfsr_xor_value;
      } while ((cindex > 71) || (cindex == prev_cindex));

      prev_cindex = cindex;

      if (cindex < 26)
      {
        // строчная буква a-z
        ch = 'a' + cindex;
        small++;
      }
      else if (cindex < 52)
      {
        // прописная буква A-Z
        ch = 'A' + cindex - 26;
        capital++;
      }
      else if (cindex < 62)
      {
        // цифра 0-9
        ch = '0' + cindex - 52;
        digit++;
      }
      else
      {
        // спецсимволы !"#$%&'() с кодами ASCII 33-41 dec
        ch = cindex - 29;
        special++;
      }
      new_password[pindex] = ch;
    }

  } while (!(small && capital && (capital < 3) && (digit == 2)));
  // в пароле должны быть как прописные, так и строчные буквы и две цифры

  new_password[len] = '\0';
}

void setNewPasswordChar(uint8_t ch, int index)
{
  if (index < PASSWORD_MAX_LENGTH)
  {
    // перезапись пароля в new_password активируется завершающим '\0'
    if ((local_password_copy[index] = ch) == '\0')
    {
      while (index >= 0)
      {
        new_password[index] = local_password_copy[index];
        index--;
      }
    }
  }
}

int savePasswordDb()
{
  eepromErase();
  return eepromWrite((uint32_t)stored_password, EEPROM_STORE_PASSWORD_OFFSET, sizeof(stored_password)) ||
         eepromWrite((uint32_t)&password_history_index, EEPROM_PASSWORD_HISTORY_INDEX_OFFSET, sizeof(password_history_index));
}

// **************************
// * запись пароля в EEPROM
// **************************

int storePassword(passwordIndex_t index)
{
  uint8_t *dst = stored_password[index];
  uint8_t *src = new_password;

  /* сохранение пароля */
  while (*dst = *src++)
    *dst++ ^= pin_code;

  /* сохранение в истории паролей */
  dst = stored_password[3 + password_history_index];
  src = new_password;
  while (*dst = *src++)
    *dst++ ^= pin_code;

  /* увеличение индекса истории паролей */
  if (++password_history_index >= PASSWORD_HISTORY_LENGTH)
    password_history_index = 0;

  return savePasswordDb();
}

void getPassword(uint8_t *dst, passwordIndex_t index)
{
  uint8_t *src = stored_password[index];

  while (*dst = *src++)
    *dst++ ^= pin_code;
}

void printNewPassword(void)
{
  typeMsg(new_password);
}

void printPassword(passwordIndex_t index)
{
  uint8_t temp_password[PASSWORD_MAX_LENGTH];
  uint8_t *dst = temp_password;
  uint8_t *src = stored_password[index];

  while (*dst = *src++)
    *dst++ ^= pin_code;

  typeMsg(temp_password);
}

void printPasswordHistory(void)
{
  int index;
  uint8_t buf[PASSWORD_MAX_LENGTH + 1];

  for (index = 0; index < PASSWORD_HISTORY_LENGTH; ++index)
  {
    getPassword(buf, index + 3);
    if (buf[0] != 0)
    {
      typeMsg(buf);
      buf[0] = ' ';
      buf[1] = (index == password_history_index) ? '*' : ' ';
      buf[2] = '\n';
      buf[3] = '\0';
      typeMsg(buf);
    }
  }
}

void clearPasswordHistory(void)
{
  memset((void *)stored_password[3], 0, PASSWORD_HISTORY_LENGTH * PASSWORD_MAX_LENGTH);
  memset((void *)&password_history_index, 0, sizeof(password_history_index));
  (void) savePasswordDb();
}

void readPasswordDb(void)
{
  eepromRead((uint32_t)&password_history_index, EEPROM_PASSWORD_HISTORY_INDEX_OFFSET, sizeof(password_history_index));
  if (password_history_index >= PASSWORD_HISTORY_LENGTH)
  {
    memset((void *)stored_password, 0, sizeof(stored_password));
    memset((void *)&password_history_index, 0, sizeof(password_history_index));
    (void) savePasswordDb();
  }

  eepromRead((uint32_t)stored_password, EEPROM_STORE_PASSWORD_OFFSET, sizeof(stored_password));
  eepromRead((uint32_t)&password_history_index, EEPROM_PASSWORD_HISTORY_INDEX_OFFSET, sizeof(password_history_index));
}
