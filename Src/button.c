#include "main.h"
#include "button.h"

#ifdef USER_BUTTON_Pin

volatile uint8_t button_down = 0;
volatile uint8_t button_up = 0;

buttonStatus_t button_status = BUTTON_STATUS_NONE;

#define buttonPressed() (HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin) == USER_BUTTON_PIN_Pressed)

void debounceButton()
{
  static uint8_t changeStateCount = 0;
  static uint8_t buttonState = 0;
  static uint8_t buttonCurrentState;

  buttonCurrentState = buttonPressed();

  if (buttonCurrentState != buttonState)
  {
    // Button state is about to be changed, increase counter
    changeStateCount++;

    if (changeStateCount >= 4)
    {
      // The button have not bounced for four checks, change state
      buttonState = buttonCurrentState;

      // If the button was pressed (not released), tell main so
      if (buttonCurrentState != 0)
      {
        button_down = 1;
        button_up = 0;
      }
      else
      {
        button_down = 0;
        button_up = 1;
      }

      changeStateCount = 0;
    }
  }
  else
  {
    // Reset counter
    changeStateCount = 0;
  }

}

#endif
