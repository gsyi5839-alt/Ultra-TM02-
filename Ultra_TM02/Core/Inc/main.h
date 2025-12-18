/* USER CODE BEGIN Header */
/**
 * @file    main.h
 * @brief   主程序头文件
 * @details Ultra-TM02超低温温度测量模块主程序
 * @version V1.0
 * @date    2025-12-18
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* 版本信息 */
#define FIRMWARE_VERSION        "V1.0"
#define FIRMWARE_DATE           "2025-12-18"
#define PRODUCT_NAME            "Ultra-TM02"
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC_DRDY_Pin GPIO_PIN_0
#define ADC_DRDY_GPIO_Port GPIOA
#define ADC_DRDY_EXTI_IRQn EXTI0_IRQn
#define ADC1_CS_Pin GPIO_PIN_4
#define ADC1_CS_GPIO_Port GPIOA
#define DAC1_CS_Pin GPIO_PIN_8
#define DAC1_CS_GPIO_Port GPIOA
#define DAC2_CS_Pin GPIO_PIN_9
#define DAC2_CS_GPIO_Port GPIOA
#define DAC1_LOAD_Pin GPIO_PIN_10
#define DAC1_LOAD_GPIO_Port GPIOA
#define DAC2_LOAD_Pin GPIO_PIN_0
#define DAC2_LOAD_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
