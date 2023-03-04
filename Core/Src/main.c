/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can_ids.h"
#include "pinout.h"
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
CAN_HandleTypeDef hcan1;

/* USER CODE BEGIN PV */
int blinker = 0;
int left = 0;
int right = 0;
int emergency = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void handle_can_fifo(uint32_t RxFifo) {

	uint8_t data[8];
	CAN_RxHeaderTypeDef header = {0,};

	if (!HAL_CAN_GetRxFifoFillLevel(&hcan1, RxFifo)) return;

	HAL_StatusTypeDef ret = HAL_CAN_GetRxMessage(
		  &hcan1,
		  RxFifo,
		  &header,
		  data
	);
	if (ret != HAL_OK) {
		Error_Handler();
		return;
	}
	switch(header.StdId) {
	case CAN_ID_BACK_UP_LIGHTS:
		setReverseLight(data[0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	case CAN_ID_BLINKER:
		blinker = data[0] ? 1 : 0;
		break;
	case CAN_ID_EMERGENCY_SWITCH:
		emergency = data[0] ? 1 : 0;
		break;
	case CAN_ID_LEFT_TURN_SIGNALS:
		left = data[0] ? 1 : 0;
		break;
	case CAN_ID_BRAKE_LIGHTS:
		setBrakeLight(data[0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	case CAN_ID_REVERSE_SWITCH:
		setReverseLight(data[0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	case CAN_ID_TAIL_LIGHTS:
		setTailLight(data[0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}
	setTurnSignalLight((blinker & left) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void handle_blink() {
	uint32_t millis = HAL_GetTick();
	static uint32_t prev = 0;
	if (millis - prev > 1000) {
		prev = millis;
		static int state = 0;
		state = !state;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, state ? GPIO_PIN_SET : GPIO_PIN_RESET); // tail light

		// For testing, send a packet at every blink
		{
		  const CAN_TxHeaderTypeDef header = {
				  .StdId = 123,
				  .IDE = CAN_ID_STD,
				  .RTR = CAN_RTR_DATA,
				  .DLC = 1, // 1 byte to send
				  .TransmitGlobalTime = DISABLE
		  };
		  static uint8_t data[] = {0x55};
		  uint32_t pTxMailbox = 0;
		  HAL_StatusTypeDef ret = HAL_CAN_AddTxMessage(&hcan1, &header, data, &pTxMailbox);
		  static uint32_t ok_count = 0;
		  static uint32_t error_count = 0;
		  static uint32_t busy_count = 0;
		  static uint32_t timeout_count = 0;
		  if (ret == HAL_OK) {
			  ok_count++;
		  } else if (ret == HAL_ERROR) {
			  error_count++;
		  } else if (ret == HAL_BUSY) {
			  busy_count++;
		  } else if (ret==HAL_TIMEOUT) {
			  timeout_count++;
		  }
		}
	}
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
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  while (1)
  {
	  HAL_CAN_IRQHandler(&hcan1);
	  //handle_blink();

	  handle_can_fifo(CAN_RX_FIFO0);
	  handle_can_fifo(CAN_RX_FIFO1);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 1;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_15TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  CAN_FilterTypeDef filter = {0,};
	/* Specifies the filter identification number (MSBs for a 32-bit
	configuration, first one for a 16-bit configuration).
	This parameter must be a number between
	Min_Data = 0x0000 and Max_Data = 0xFFFF. */
	filter.FilterIdHigh =  0; // MY_STID << 5;

	/* Specifies the filter identification number (LSBs for a 32-bit
	configuration, second one for a 16-bit configuration).
	This parameter must be a number between
	Min_Data = 0x0000 and Max_Data = 0xFFFF. */
	filter.FilterIdLow = CAN_RTR_DATA | CAN_ID_STD;

	/* Specifies the FIFO (0 or 1U) which will be assigned to the filter.
	This parameter can be a value of @ref CAN_filter_FIFO */
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;

	/* Specifies the filter bank which will be initialized.
	For single CAN instance(14 dedicated filter banks),
	this parameter must be a number between Min_Data = 0 and Max_Data = 13.
	For dual CAN instances(28 filter banks shared),
	this parameter must be a number between Min_Data = 0 and Max_Data = 27. */
	filter.FilterBank = 0;

	/* Specifies the filter mode to be initialized.
	This parameter can be a value of @ref CAN_filter_mode */
	filter.FilterMode = CAN_FILTERMODE_IDMASK;

	/*!< Specifies the filter scale.
	This parameter can be a value of @ref CAN_filter_scale */
	filter.FilterScale = CAN_FILTERSCALE_32BIT;

	/* Enable or disable the filter.
	This parameter can be a value of @ref CAN_filter_activation */
	filter.FilterActivation = CAN_FILTER_ENABLE;

	/* Select the start filter bank for the slave CAN instance.
	For single CAN instances, this parameter is meaningless.
	For dual CAN instances, all filter banks with lower index are assigned to master
	CAN instance, whereas all filter banks with greater index are assigned to slave
	CAN instance.
	This parameter must be a number between Min_Data = 0 and Max_Data = 27. */
	filter.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&hcan1, &filter) != HAL_OK) {
	  Error_Handler();
  }
//  if (HAL_CAN_ActivateNotification(
//		  &hcan1,
//		  /* Transmit Interrupt */
//		  CAN_IT_TX_MAILBOX_EMPTY |
//		  /* Operating Mode Interrupts */
//		  CAN_IT_WAKEUP |
//		  CAN_IT_SLEEP_ACK |
//		  /* Error Interrupts */
//		  CAN_IT_ERROR_WARNING |
//		  CAN_IT_ERROR_PASSIVE |
//		  CAN_IT_BUSOFF |
//		  CAN_IT_LAST_ERROR_CODE |
//		  CAN_IT_ERROR
//  ) != HAL_OK) {
//	  Error_Handler();
//  }
  /* USER CODE END CAN1_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA4 PA6 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB4 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
