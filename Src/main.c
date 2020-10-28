/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "button.h"
#include "eeprom_emu.h"
#include "keyboard_out.h"
#include "keyboard_in.h"
#include "led.h"
#include "password.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static passwordIndex_t password_index;
static uint8_t genpass_len;

// ***********************************************
// * текстовые сообщения для вывода "на консоль"
// ***********************************************

const uint8_t msg_passwdgen_start[]  = "Generating...";
const uint8_t msg_passwdgen_finish[] = " done.";
const uint8_t msg_passwd_stored[]    = "Stored in EEPROM.";
const uint8_t msg_history_header[]   = "History of passwords:\n";
const uint8_t msg_history_erased[]   = "History erased.";
const uint8_t msg_history_empty[]    = "Password history empty.";

void softReset(void)
{
  pin_code = 0;
  ledBlink(25);

  while (keyCommandState() != KEY_CMD_STATE_PIN)
    USBD_Delay(1);

  /* ожидание пин кода */
  uint16_t keySeq = keyCommandSequence();

  for (int i = 0; i < 8; i++)
  {
    switch (keySeq & 0b11)
    {
    case CAPS_CODE:
      break;                // CAPS интерпретируется как 0
    case NUM_CODE:
    case SCROLL_CODE:
      pin_code |= (1 << i); // NUM & SCROLL интерпретируется как 1
    default:
      break;
    }
    keySeq >>= 2;
  }

  initRandomGenerator();
  keyCommandReset();

  ledBlink(5);
  USBD_Delay(500);
  ledBlink(0);
}

void checkButtonReset(void)
{
#ifdef USER_BUTTON_Pin
  if (button_down > 0)
  {
    if (button_status == BUTTON_STATUS_NONE) // кнопка нажата без необходимости подтверждать операцию
    {
      button_status = BUTTON_STATUS_PRESSED;
      simple_delay_counter = 150; // 3 секунды до сброса PIN кода
    }
    else if (button_status == BUTTON_STATUS_PRESSED && simple_delay_counter == 0)
    {
      softReset();
      button_status = BUTTON_STATUS_NONE;
    }
  }
  else if (button_status == BUTTON_STATUS_PRESSED)
  {
    button_status = BUTTON_STATUS_NONE;
  }
#endif
}

void checkButtonConfirm(void)
{
#ifdef USER_BUTTON_Pin
  if (button_status == BUTTON_STATUS_WAITING_FOR_CONFIRMATION)
  {
    if (simple_delay_counter == 0)
    {
      button_status = BUTTON_STATUS_TIMEOUT; // время ожидания нажатия вышло
    }
    // по мере уменьшения simple_delay_counter мигаем чаще и чаще
    else
    {
      if (simple_delay_counter < 50)
      {
        ledBlink(2);
      }
      else if (simple_delay_counter < 100)
      {
        ledBlink(5);
      }
      else if (simple_delay_counter < 150)
      {
        ledBlink(10);
      }

      if (button_down > 0)
      {
        button_down = 0;
        button_status = BUTTON_STATUS_CONFIRMED;
      }
    }
  }
#endif
}

void buttonConfirm(void _fn(void))
{

#ifdef USER_BUTTON_Pin
  if (button_status == BUTTON_STATUS_NONE) // ... первый вход
  {
    button_status = BUTTON_STATUS_WAITING_FOR_CONFIRMATION;
    simple_delay_counter = 150; // 3 секунды
    button_down = 0;
  }

  if (button_status != BUTTON_STATUS_WAITING_FOR_CONFIRMATION) // ... второй и последующие входы
  {
    if (button_status == BUTTON_STATUS_CONFIRMED)
    {
#endif

      _fn();

#ifdef USER_BUTTON_Pin
    }
    button_status = BUTTON_STATUS_NONE;
#endif

    ledBlink(0);
    keyCommandReset();

#ifdef USER_BUTTON_Pin
  }
#endif
}
void cmdStorePassword(void)
{
  ledBlink(3);
  storePassword(password_index);

  // печатаем сообщение, что новый пароль сохранен в EEPROM
  typeMsg((uint8_t *)msg_passwd_stored);
}

void cmdPrintHistory(void)
{
  ledBlink(5);
  if (stored_password[3][0] != '\0')
  {
    typeMsg((uint8_t *)msg_history_header);
    printPasswordHistory();
  }
  else
  {
    typeMsg((uint8_t *)msg_history_empty);
  }
}

void cmdClearHistory(void)
{
  ledBlink(5);
  clearPasswordHistory();
  typeMsg((uint8_t *)msg_history_erased);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_TIM2_Init();

  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_Base_Start_IT(&htim2);

  readPasswordDb();
  waitKbdLedState();
  softReset();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

#ifdef USER_BUTTON_Pin
    debounceButton();
    checkButtonReset();
#endif

    if (keyCommandState() != KEY_CMD_STATE_READY)
      continue;

#ifdef USER_BUTTON_Pin
    checkButtonConfirm();
#endif

    switch (keyCommandSequence())
    {

    /* набор паролей */
    case KEY_COMMAND_TYPEPASS_CAPS:
      password_index = PASSWD_CAPS;
      goto TYPEPASS;
    case KEY_COMMAND_TYPEPASS_NUM:
      password_index = PASSWD_NUM;
      goto TYPEPASS;
    case KEY_COMMAND_TYPEPASS_SCROLL:
      password_index = PASSWD_SCROLL;

    TYPEPASS:
      ledBlink(5);
      printPassword(password_index);
      goto CMD_DONE;

    /* генерация паролей */
    case KEY_COMMAND_GENPASS:
      genpass_len = 8;
      goto GENPASS;
    case KEY_COMMAND_GENPASS_12:
      genpass_len = 12;
      goto GENPASS;
    case KEY_COMMAND_GENPASS_15:
      genpass_len = 15;

    GENPASS:
      ledBlink(3);
      typeMsg((uint8_t *)msg_passwdgen_start);
      generatePassword(genpass_len);
      typeMsg((uint8_t *)msg_passwdgen_finish);
      goto CMD_DONE;

    case KEY_COMMAND_TYPENEWPASS:
      ledBlink(5);
      printNewPassword();
      goto CMD_DONE;

    /* сохранение паролей во флеш-памяти */
    case KEY_COMMAND_STORPASS_CAPS:
      password_index = PASSWD_CAPS;
      goto STORPASS;
    case KEY_COMMAND_STORPASS_NUM:
      password_index = PASSWD_NUM;
      goto STORPASS;
    case KEY_COMMAND_STORPASS_SCROLL:
      password_index = PASSWD_SCROLL;

    STORPASS:
      buttonConfirm(cmdStorePassword);
      break;

    /* печать истории сохранённых паролей */
    case KEY_COMMAND_PRINT_HISTORY:
      buttonConfirm(cmdPrintHistory);
      break;

    /* очистка истории сохранённых паролей */
    case KEY_COMMAND_CLEAR_HISTORY:
      buttonConfirm(cmdClearHistory);
      break;

    CMD_DONE:
    default:
      ledBlink(0);
      keyCommandReset();
      break;
    }

    USBD_Delay(5);
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 35999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 39;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, USER_LED_PIN_Off);

  /*Configure GPIO pin : USER_LED_Pin */
  GPIO_InitStruct.Pin = USER_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USER_LED_GPIO_Port, &GPIO_InitStruct);

#ifdef USER_BUTTON_Pin
  /*Configure GPIO pin : USER_BUTTON_Pin */
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = USER_BUTTON_GPIO_Pull;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);
#endif

#ifdef USB_DISC_Pin
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_DISC_GPIO_Port, USB_DISC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USB_DISC_Pin */
  GPIO_InitStruct.Pin = USB_DISC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_DISC_GPIO_Port, &GPIO_InitStruct);
#endif
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler().
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2)
  {
    simpleDelayCounterTask();
    keyCommandTimerTask();
    ledBlinkTask();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
