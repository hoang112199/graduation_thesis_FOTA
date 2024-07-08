/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "uart.h"
#include "task_LTE.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MaxLengthReceiveUSART1 1024
#define LastFOTACommand 15
#define WaitingForRespondTimeout 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osMutexId myMutex01Handle;
/* USER CODE BEGIN PV */
/* Check command from sim variable */
osThreadId taskCheckSMSHandle;
/* RxData uart 1 buffer variables */
char RxDataUSART1[MaxLengthReceiveUSART1];
char RxDataChar[1];
volatile uint16_t RxDataIndex = 0;
/* RxData uart 6 buffer variables */
char RxDataUSART6[1024];
char RxDataCharSim[1];
volatile uint16_t RxDataIndexSim = 0;
/* Flag check if uart 6 received completed respond or not */
int isReceivedRespond = 0;
/* Flag check if in FOTA mode or not */
int FOTAFlag = 0;
/* Flag to make the received command from sim different from FOTA process */
int SMSCommandFlag = 1;
/* Flag check if command is sent to SIM or not */
volatile int sentToSimFlag = 0;
/* Command index */
volatile int FOTACommandIndex = 0;
/* Time out variable for FOTA */
volatile int SimRespondTimeout = 0;
/* Flag check if the Firmware is downloaded or not */
int writeToFlash = 0;
/* Start page of where to write firmware */
int beginLine = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);

/* USER CODE BEGIN PFP */
void TaskCheckCommandFromUser(void const * argument);
void printDebug(char *debugText);
void erase_flash_sector(uint32_t sector_number);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void changeSettingBeforeReset();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* Change vector offset table to the start of the firmware */
  SCB->VTOR = (uint32_t)0x08060000;
	__enable_irq();
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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  printDebug("Firmware version 0.2.3\r\n");
  uint8_t tx_data_1[37]="Start receive and process firmware\r\n";
  HAL_UART_Transmit(&huart6,tx_data_1, sizeof(tx_data_1), 100);
  HAL_UART_Receive_IT(&huart6, (uint8_t*)RxDataChar, 1);
  HAL_UART_Receive_IT(&huart1, (uint8_t*)RxDataCharSim, 1);
  /* USER CODE END 2 */
  /* Create the mutex(es) */
  /* definition and creation of myMutex01 */
  osMutexDef(myMutex01);
  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityIdle, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  
  osThreadDef(taskCheckSMS, TaskCheckCommandFromUser, osPriorityIdle, 0, 128);
  taskCheckSMSHandle = osThreadCreate(osThread(taskCheckSMS), NULL);

  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  htim2.Init.Prescaler = 1599;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9999;
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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART6)
 { 
	 uint8_t RxLength = 0;
	 RxDataUSART6[RxDataIndex++] = RxDataChar[0];
    if (RxDataChar[0] == '\n')
    {
      RxLength = RxDataIndex - 1;
      if(RxDataUSART6[0] == 'F' && RxDataUSART6[1] == 'O' &&
        RxDataUSART6[2] == 'T' && RxDataUSART6[3] == 'A' && sentToSimFlag == 0)
      {
        printDebug("Enter FOTA mode\r\n");
        FOTAFlag = 1;
        writeToFlash = 0;
        SMSCommandFlag = 0;
        sendUserWarning();
      }     
      RxDataIndex = 0;
      HAL_UART_Receive_IT(&huart6, (uint8_t*)RxDataChar, 1);
    }
    else
    {
      HAL_UART_Receive_IT(&huart6, (uint8_t*)RxDataChar, 1);
    }	 
 }
 else if(huart->Instance == USART1)
 { 
	 uint8_t RxLength = 0;
	 RxDataUSART1[RxDataIndexSim++] = RxDataCharSim[0];
    HAL_UART_Receive_IT(&huart1, (uint8_t*)RxDataCharSim, 1);

   if(SMSCommandFlag == 0)
   {
     if(RxDataCharSim[0] == 'O')
     {
       isReceivedRespond = 1; // need replace this condition another time
     }
   }  
 }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    
}

void printDebug(char *debugText)
{
	HAL_UART_Transmit(&huart6,(uint8_t *)debugText,(uint16_t)strlen(debugText),100);
}

void erase_flash_sector(uint32_t sector_number) 
{
    FLASH_EraseInitTypeDef erase_init;
    uint32_t sector_error = 0;

    // Fill erase structure
    erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase_init.Sector = sector_number;
    erase_init.NbSectors = 1;
    erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3; 

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    HAL_FLASHEx_Erase(&erase_init, &sector_error); 
    HAL_FLASH_Lock();
}

void changeSettingBeforeReset()
{
  uint8_t readConfig = *(uint8_t*)CONFIG_START_ADDRESS;
  if(readConfig == '5')
  {
    erase_flash_sector(FLASH_SECTOR_2);
    HAL_FLASH_Unlock();   
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                             FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    // Config save in sector 3
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, (StartPageOfConfig*1024), '7');
    HAL_FLASH_Lock();
  }
  else if (readConfig == '7')
  {
    erase_flash_sector(FLASH_SECTOR_2);
    HAL_FLASH_Unlock();    
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                             FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    // Config save in sector 3
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, (StartPageOfConfig*1024), '5');
    HAL_FLASH_Lock();
  }
  
}

void TaskCheckCommandFromUser(void const * argument)
{
  char expectRespond1[] = "+CMTI: \"SM\"";
  char expectRespond2[] = "FOTA";
  char expectedPhoneNumber[] = PhoneNumber;
  for(;;)
  {
    if(SMSCommandFlag == 1)
    {
      if(strstr(RxDataUSART1, expectRespond1) != NULL)
      {
        printDebug("Checking new received SMS\r\n");
        deleteBuffer(RxDataUSART1);
        RxDataIndexSim = 0;
        SendCommandToSim("AT+CMGL=\"ALL\"");
        osDelay(2000);
        if(strstr(RxDataUSART1, expectRespond2) != NULL)        
        {
          if(strstr(RxDataUSART1, expectedPhoneNumber) != NULL)
          {
            SendCommandToSim("AT+CMGD=1,1");
            osDelay(500);
            deleteBuffer(RxDataUSART1);
            RxDataIndexSim = 0;         
            FOTAFlag = 1;
            writeToFlash = 0;
            SMSCommandFlag = 0;
          }
          else
          {
            
          }
        }
      }
    }   
    osDelay(5000);
  }
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    if(SMSCommandFlag == 0)
    {
      if(FOTAFlag == 1 && sentToSimFlag == 0)
      {
        CheckSimConditionCommand(FOTACommandIndex);
        SimRespondTimeout = 0;
        sentToSimFlag = 1;
      }
      osDelay(1000);
      if(sentToSimFlag == 1)
      {
        SimRespondTimeout++;
        if(SimRespondTimeout == WaitingForRespondTimeout)
        {
          sentToSimFlag = 0;
          FOTACommandIndex = 0;
          FOTAFlag = 0;
          SimRespondTimeout = 0;
          printDebug("Time out\r\n");
          SMSCommandFlag = 1;
          if(writeToFlash == 0)
          {
            SendCommandToSim("AT+CFTPSLOGOUT");
            osDelay(1000);
            SendCommandToSim("AT+CFTPSSTOP");
          }
        }
      }
      if(isReceivedRespond == 1)
      { 
        char expectRespond1[] = "OK";
        char expectRespond2[] = "ERROR";
        if(strstr(RxDataUSART1, expectRespond1) != NULL
          || strstr(RxDataUSART1, expectRespond2) != NULL)
        {     
          printDebug(RxDataUSART1);
          if(writeToFlash == 0)
          {
            if(processRespond(RxDataUSART1, FOTACommandIndex) == 1)
            {
              deleteBuffer(RxDataUSART1);
              RxDataIndexSim = 0;
              isReceivedRespond = 0;
              sentToSimFlag = 0;
              FOTACommandIndex++;
              if(FOTACommandIndex == 5 ||FOTACommandIndex == 10 || FOTACommandIndex == 11)
              {
                printDebug("Waiting respond.\r\n"); // Wait 5s for log in and download
                osDelay(5000);
              }
              if(FOTACommandIndex == LastFOTACommand)
              {
                printDebug("Complete download firmware. Start writing to flash.\r\n");
                HAL_FLASH_Unlock();
                HAL_FLASH_Lock();
                uint8_t readConfig = *(uint8_t*)CONFIG_START_ADDRESS;
                if(readConfig == '5')
                {
                  erase_flash_sector(FLASH_SECTOR_7);
                  printDebug("Erase old flash done\r\n");
                  writeToFlash = 1;
                  FOTACommandIndex = 0;
                  FOTAFlag = 0;
                  setStartPage(StartPageOfFirmware2);
                  SIM_RequestFirmwareCommand(beginLine); 
                }
                else if(readConfig == '7')
                {              
                  erase_flash_sector(FLASH_SECTOR_5);
                  erase_flash_sector(FLASH_SECTOR_6);
                  printDebug("Erase old flash done\r\n");
                  writeToFlash = 1;
                  FOTACommandIndex = 0;
                  FOTAFlag = 0;
                  setStartPage(StartPage);
                  SIM_RequestFirmwareCommand(beginLine); 
                }
                else
                {
                  SMSCommandFlag = 1;
                  printDebug("Error erase flash");
                  sendFailNotification();
                }     
                HAL_FLASH_Unlock();
                HAL_FLASH_Lock();              
              }
            }
            else
            {
              deleteBuffer(RxDataUSART1);
              RxDataIndexSim = 0;
              isReceivedRespond = 0;
              sentToSimFlag = 0;
              printDebug("Error\r\n");
              FOTAFlag = 0;
              FOTACommandIndex = 0;
              SMSCommandFlag = 1;
              SendCommandToSim("AT+CFTPSLOGOUT");
              osDelay(1000);
              SendCommandToSim("AT+CFTPSSTOP");
              sendFailNotification();
            }          
          }
          else if(writeToFlash == 1)
          {
            int result = ProcessFirmwareReceived(RxDataUSART1);
            deleteBuffer(RxDataUSART1);
            RxDataIndexSim = 0;
            isReceivedRespond = 0;
            if(result == 1)
            {
              printDebug("Error during write data to flash.\r\n");
              writeToFlash = 0;
              beginLine = 0;
              SMSCommandFlag = 1;
              sendFailNotification();
            }         
            else if(result == 2)
            {
              printDebug("Complete write to flash. Start restart device.\r\n");
              writeToFlash = 0;
              beginLine = 0;
              SMSCommandFlag = 1;
              changeSettingBeforeReset();
              sendSuccessNotification();
              NVIC_SystemReset();
            }
            else if(result == 0)
            {
              beginLine += 512;
              osDelay(500);
              SIM_RequestFirmwareCommand(beginLine);
            }
            else if(result == 3)
            {
              osDelay(500);
              printDebug("Receive unwanted respond.\r\n");
              SIM_RequestFirmwareCommand(beginLine);
            }
          }         
        }   
      }   
    }    
    osDelay(10);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
    printDebug("Task 2.3\r\n");
    osDelay(5000);
  }
  /* USER CODE END StartTask02 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  else if (htim->Instance == TIM2)
    {
        // Timeout occurred
        HAL_TIM_Base_Stop(&htim2);
        RxDataIndexSim = 0;
        // Handle timeout error here
        display("error\r\n");
    }
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
