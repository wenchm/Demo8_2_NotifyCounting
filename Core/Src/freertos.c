/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "usart.h"
#include "keyled.h"
#include "rtc.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_CheckIn */
osThreadId_t Task_CheckInHandle;
const osThreadAttr_t Task_CheckIn_attributes = {
  .name = "Task_CheckIn",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_CheckIn(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

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
  /* creation of Task_CheckIn */
  Task_CheckInHandle = osThreadNew(AppTask_CheckIn, NULL, &Task_CheckIn_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_CheckIn */
/**
  * @brief  Function implementing the Task_CheckIn thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_CheckIn */
void AppTask_CheckIn(void *argument)
{
	/* USER CODE BEGIN AppTask_CheckIn */
	/* Infinite loop */
	for(;;)
	{
		KEYS curKey=ScanPressedKey(20);
		if (curKey==KEY_RIGHT)  //KeyRight pressed
		{
			BaseType_t clearOnExit=pdFALSE;						//退出时通知值减1
			// 只是在通知值为0时才进入阻塞状态，所以可以多次读取通知值，每次使通知值减1
			BaseType_t preCount=ulTaskNotifyTake(clearOnExit, portMAX_DELAY);
			// BaseType_t preCount=ulTaskNotifyTake(clearOnExit, pdMS_TO_TICKS(500));

			printf("People in waiting= %ld\r\n",preCount-1);		// preCount是前一次的通知值
			vTaskDelay(pdMS_TO_TICKS(300));  					//延时，消除按键抖动影响
		}
		else
			vTaskDelay(pdMS_TO_TICKS(5));
	}
  /* USER CODE END AppTask_CheckIn */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* RTC周期唤醒中断回调函数 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	LED1_Toggle();								//LED1闪烁
	BaseType_t taskWoken=pdFALSE;
	vTaskNotifyGiveFromISR(Task_CheckInHandle,&taskWoken);  //发送通知，通知值加1
	portYIELD_FROM_ISR(taskWoken); //必须执行这条语句，申请任务调度
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart3,(uint8_t*)&ch,1,0xFFFF);
	return ch;
}
/* USER CODE END Application */

