#ifndef __PASSWORD_H
#define __PASSWORD_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
  PASSWD_CAPS,
  PASSWD_NUM,
  PASSWD_SCROLL
} passwordIndex_t;

#define PASSWORD_MAX_LENGTH 16
#define PASSWORD_HISTORY_LENGTH 13 // количество запоминаемых в "истории" паролей
#define VENDOR_REQ_SET_PASSWORD_CHAR 0x01

extern uint8_t stored_password[3 + PASSWORD_HISTORY_LENGTH][PASSWORD_MAX_LENGTH];
extern uint32_t password_history_index;

#define EEPROM_STORE_PASSWORD_OFFSET 0
#define EEPROM_PASSWORD_HISTORY_INDEX_OFFSET (EEPROM_STORE_PASSWORD_OFFSET + sizeof(stored_password))

extern uint8_t pin_code;

void initRandomGenerator(void);
void generatePassword(uint8_t len);
void setNewPasswordChar(uint8_t ch, int index);
int storePassword(passwordIndex_t index);
void getPassword(uint8_t *destination, passwordIndex_t index);
void printNewPassword(void);
void printPassword(passwordIndex_t index);
void printPasswordHistory(void);
void clearPasswordHistory(void);
void readPasswordDb(void);

#ifdef __cplusplus
}
#endif

#endif