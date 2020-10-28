#include "main.h"
#include "eeprom_emu.h"

#define EEPROM_START_ADDR FLASH_BASE + 1024 * 127 // TODO использовать регистр FLASHSIZE_BASE

#ifdef HAL_FLASH_MODULE_ENABLED

static int _flash_erase(uint32_t address)
{
  static FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t page_error = 0;

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = address;
  EraseInitStruct.NbPages = 1;
  // EraseInitStruct.Banks = FLASH_BANK_1; // FLASH_BANK_2 - банк 2, FLASH_BANK_BOTH - оба банка

  HAL_FLASH_Unlock();
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &page_error) != HAL_OK)
    ;

  HAL_FLASH_Lock();
  return page_error != 0xFFFFFFFF;
}

int eepromWrite(uint32_t src, int offset, int size)
{
  uint32_t dst = EEPROM_START_ADDR + offset;
  int retval = 0;

  HAL_FLASH_Unlock();
  while (size > 0)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, dst, *(__IO uint16_t *)src) != HAL_OK)
    {
      retval = 1;
      break;
    }

    size -= 2;

    dst += 2;
    src += 2;
  }
  HAL_FLASH_Lock();

  return retval;
}

#else

static int inline _flash_busy(void)
{
  return FLASH->SR & FLASH_SR_BSY;
}

static void inline _flash_enable(void)
{
  FLASH->CR |= FLASH_CR_PG;
}

static void inline _flash_disable(void)
{
  FLASH->CR &= ~(FLASH_CR_PG);
}

static void inline _flash_unlock(void)
{
  if (FLASH->CR & FLASH_CR_LOCK)
  {
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
  }
}

static void inline _flash_lock(void)
{
  FLASH->CR |= FLASH_CR_LOCK;
}

static int _flash_eop(void)
{
  if (FLASH->SR & FLASH_SR_EOP)
  {
    FLASH->SR |= FLASH_SR_EOP;
    return 1;
  }

  return 0;
}

static int _flash_erase(uint32_t address)
{
  while (_flash_busy())
    ;                        // ожидание готовности флеша
  FLASH->CR |= FLASH_CR_PER; // бит стирания одной страницы

  FLASH->AR = address;        // адрес страницы
  FLASH->CR |= FLASH_CR_STRT; // запуск стирания

  while (_flash_busy())
    ;                         // ожидание завершения стирания (готовности)
  FLASH->CR &= ~FLASH_CR_PER; // сброс бита стирания одной страницы

  return !_flash_eop(); // операция завершена, очищаем флаг
}

int eepromWrite(uint32_t src, int offset, int size)
{
  uint32_t dst = EEPROM_START_ADDR + offset;

  _flash_unlock();

  while (size > 0)
  {
    while (_flash_busy())
      ;
    _flash_enable();

    *(__IO uint16_t *)dst = *(__IO uint16_t *)src;

    while (_flash_busy())
      ;
    if (!_flash_eop())
      return 1;

    _flash_disable();

    size -= 2;

    dst += 2;
    src += 2;
  }

  while (_flash_busy())
    ;
  _flash_lock();

  return 0;
}

#endif

void eepromErase(void)
{
  _flash_erase(EEPROM_START_ADDR);
}

void eepromRead(uint32_t dst, int offset, int size)
{
  uint32_t src = EEPROM_START_ADDR + offset;

  while (size--)
  {
    *(__IO uint8_t *)dst++ = *(__IO uint8_t *)src++;
  }
}
