#include "main.h"
#include "led.h"

volatile uint8_t blinkDelay = 0;
volatile uint8_t blinkDelayCounter = 0;
volatile uint8_t simple_delay_counter = 0;

void ledOn(void)
{
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, USER_LED_PIN_On);
}

void ledOff(void)
{
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, USER_LED_PIN_Off);
}

void ledToggle(void)
{
  HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
}

void ledBlink(uint8_t delay)
{
  if (delay == blinkDelay)
    return;

  if (delay == 0)
    ledOff();
  else
    ledOn();

  blinkDelay = blinkDelayCounter = delay;
}

void ledBlinkTask(void)
{
  if (blinkDelay == 0)
    return;

  if (blinkDelayCounter-- == 0)
  {
    ledToggle();
    blinkDelayCounter = blinkDelay;
  }
}

void simpleDelayCounterTask(void)
{
  if (simple_delay_counter > 0)
    --simple_delay_counter;
}
