/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "gpio.h"
#include "w25qxx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern TIM_HandleTypeDef htim1;
extern ETH_HandleTypeDef heth;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Boot Jump To App Address */
#define VECT_TAB_OFFSET       0x00000000UL
#define APPLICATION_ADDRESS   (uint32_t)0x90000000
/* Trace Buffer Size */
#define TRC_BUF_SIZE        (1024*32)
#define TRC_MAX_OBJ_COUNT   (40)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* Trace Buffer */
UCHAR Trace_Buffer[TRC_BUF_SIZE] = { 0 };
/* Boot Jump To App Function */
typedef void (*pFunction)(void);
pFunction JumpToApplication;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
TX_THREAD thread_0;
UCHAR thread_0_stack[1024] = { 0 };
void thread_0_entry(ULONG thread_input);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN App_ThreadX_MEM_POOL */
  (void)byte_pool;
  /* USER CODE END App_ThreadX_MEM_POOL */

  /* USER CODE BEGIN App_ThreadX_Init */
  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0, thread_0_stack, sizeof(thread_0_stack), 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  MX_ThreadX_Init
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
static void boot_environment_reset(void)
{
    uint8_t index = 0;
    /* Disable global interrupts */
    __set_PRIMASK(1);
    /* Disable ICache and DCache */
    SCB_DisableICache();
    SCB_DisableDCache();
    /* Disable all interrupts, clear all interrupt pending flags */
    for (index = 0; index < 8; index++)
    {
        NVIC->ICER[index]=0xFFFFFFFF;
        NVIC->ICPR[index]=0xFFFFFFFF;
    }
    /* Tick timer count clear */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;
}

void boot_jump_to_app(void)
{
  /* Enable memory mapped mode, run on external flash */
  W25QXX_Init();
  W25Q_Memory_Mapped_Enable();
  /* Boot environment deinit */
  boot_environment_reset();
  /* The first address + 4 is the address of the reset interrupt service routine */
  JumpToApplication = (pFunction)(*(__IO uint32_t *)(APPLICATION_ADDRESS + 4));
  /* Set Main Stack Pointer */
  __set_PSP(*(__IO uint32_t *)APPLICATION_ADDRESS);
  __set_MSP(*(__IO uint32_t *)APPLICATION_ADDRESS);
  /* Set to privileged mode, use MSP pointer */
  __set_CONTROL(0);
  /* Jump to APP */
  JumpToApplication();
}

void thread_0_entry(ULONG thread_input)
{
  static uint32_t time = 3;
  
	while(1)
	{
		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_SET);
		tx_thread_sleep(750);
		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_RESET);
		tx_thread_sleep(250);
    /* delay 5 seconds jump to app */
    printf("time remaining:%d\r\n", time);
    time = time - 1;
    if(time == 0)
    {
      printf("now start jump to application!\r\n");
      boot_jump_to_app();
    }
	}
}
/* USER CODE END 1 */
