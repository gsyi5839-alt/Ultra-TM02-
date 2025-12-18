/* USER CODE BEGIN Header */
/**
 * @file    main.c
 * @brief   主程序文件
 * @details Ultra-TM02超低温温度测量模块主程序
 * @version V1.0
 * @date    2025-12-18
 * 
 * @attention
 * 系统配置:
 * - MCU: STM32F411RET6 (100MHz)
 * - HSE: 12MHz外部晶振
 * - USB: 48MHz (CDC虚拟串口)
 * - SPI1: 12.5MHz (ADC/DAC通讯)
 * - USART6: 115200 (LCD串口屏)
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* BSP层头文件 */
#include "bsp_gpio.h"
#include "bsp_spi.h"
#include "bsp_uart.h"
#include "bsp_flash.h"

/* Service层头文件 */
#include "svc_adc.h"
#include "svc_dac.h"
#include "svc_lcd.h"
#include "svc_usb.h"

/* App层头文件 */
#include "app_temp.h"
#include "app_param.h"
#include "app_comm.h"
#include "app_output.h"
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

/* USER CODE BEGIN PV */
static uint32_t led_tick = 0;       /* LED闪烁计时 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void App_Init(void);
static void App_Process(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief  应用层初始化
 */
static void App_Init(void)
{
    /* BSP层初始化 */
    BSP_GPIO_Init();        /* GPIO初始化 (片选、LED等) */
    
    /* Service层初始化 */
    SVC_ADC_Init();         /* ADC服务初始化 */
    SVC_DAC_Init();         /* DAC服务初始化 */
    SVC_LCD_Init();         /* LCD服务初始化 */
    SVC_USB_Init();         /* USB服务初始化 */
    
    /* 显示启动信息 */
    SVC_LCD_SetStatus("Starting...");
    HAL_Delay(500);
    
    /* App层初始化 */
    APP_Param_Init();       /* 参数管理初始化 (从Flash加载参数) */
    APP_Temp_Init();        /* 温度测量初始化 */
    APP_Output_Init();      /* 4-20mA输出初始化 */
    APP_Comm_Init();        /* 通讯协议初始化 */
    
    /* 启动完成，更新显示 */
    SVC_LCD_SetStatus("Ready");
    SVC_LCD_SetCurrentSource(APP_Param_GetCurrentSource());
    
    /* 自动开始测量 */
    APP_Temp_Start();
}

/**
 * @brief  应用层主循环处理
 */
static void App_Process(void)
{
    /* 温度测量处理 */
    APP_Temp_Process();
    
    /* 通讯协议处理 (USB接收) */
    APP_Comm_Process();
    
    /* LCD显示更新 */
    SVC_LCD_Update();
    
    /* LED闪烁 (1Hz) - 指示系统运行 */
    if (HAL_GetTick() - led_tick >= 500)
    {
        led_tick = HAL_GetTick();
        BSP_LED_Toggle();
    }
    
    /* 4-20mA输出更新 (如果测量正在运行) */
    if (APP_Temp_IsRunning())
    {
        APP_Output_UpdateCurrent(APP_Temp_GetValue());
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
  MX_SPI1_Init();
  MX_USART6_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  /* 应用层初始化 */
  App_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* 应用层主循环处理 */
    App_Process();
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  /* 禁用中断 */
  __disable_irq();
  
  /* LED快闪指示错误 */
  while (1)
  {
    BSP_LED_Toggle();
    for (volatile uint32_t i = 0; i < 500000; i++);
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
  /* 用户可在此添加自己的处理代码 */
  (void)file;
  (void)line;
  Error_Handler();
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
