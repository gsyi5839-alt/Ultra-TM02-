/**
 * @file    bsp_gpio.h
 * @brief   GPIO板级支持包头文件
 * @details 提供ADC/DAC片选、LOAD信号、LED控制等GPIO操作接口
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 宏定义 --------------------------------------------------------------------*/

/**
 * 引脚定义说明：
 * 以下宏使用main.h中CubeIDE生成的引脚定义
 * 如果main.h中没有定义，则使用备用定义
 */

/* ADC1片选引脚定义 (PA4) */
#ifndef ADC1_CS_Pin
#define ADC1_CS_Pin         GPIO_PIN_4
#define ADC1_CS_GPIO_Port   GPIOA
#endif

/* DAC1片选引脚定义 (PA8) */
#ifndef DAC1_CS_Pin
#define DAC1_CS_Pin         GPIO_PIN_8
#define DAC1_CS_GPIO_Port   GPIOA
#endif

/* DAC2片选引脚定义 (PA9) */
#ifndef DAC2_CS_Pin
#define DAC2_CS_Pin         GPIO_PIN_9
#define DAC2_CS_GPIO_Port   GPIOA
#endif

/* DAC1加载信号引脚定义 (PA10) */
#ifndef DAC1_LOAD_Pin
#define DAC1_LOAD_Pin       GPIO_PIN_10
#define DAC1_LOAD_GPIO_Port GPIOA
#endif

/* DAC2加载信号引脚定义 (PB0) */
#ifndef DAC2_LOAD_Pin
#define DAC2_LOAD_Pin       GPIO_PIN_0
#define DAC2_LOAD_GPIO_Port GPIOB
#endif

/* LED1引脚定义 (PC13) */
#ifndef LED1_Pin
#define LED1_Pin            GPIO_PIN_13
#define LED1_GPIO_Port      GPIOC
#endif

/* ADC数据就绪引脚定义 (PA0) - 外部中断输入 */
#ifndef ADC_DRDY_Pin
#define ADC_DRDY_Pin        GPIO_PIN_0
#define ADC_DRDY_GPIO_Port  GPIOA
#endif

/* 函数声明 ------------------------------------------------------------------*/

/**
 * @brief  GPIO初始化
 * @note   初始化所有控制引脚，设置默认状态
 * @retval 无
 */
void BSP_GPIO_Init(void);

/**
 * @brief  设置ADC1片选信号
 * @param  state: 0=选中(低电平), 1=取消选中(高电平)
 * @retval 无
 */
void BSP_ADC_CS(uint8_t state);

/**
 * @brief  设置DAC1片选信号
 * @param  state: 0=选中(低电平), 1=取消选中(高电平)
 * @retval 无
 */
void BSP_DAC1_CS(uint8_t state);

/**
 * @brief  设置DAC2片选信号
 * @param  state: 0=选中(低电平), 1=取消选中(高电平)
 * @retval 无
 */
void BSP_DAC2_CS(uint8_t state);

/**
 * @brief  设置DAC1加载信号
 * @param  state: 0=低电平(触发加载), 1=高电平(正常)
 * @retval 无
 */
void BSP_DAC1_LOAD(uint8_t state);

/**
 * @brief  设置DAC2加载信号
 * @param  state: 0=低电平(触发加载), 1=高电平(正常)
 * @retval 无
 */
void BSP_DAC2_LOAD(uint8_t state);

/**
 * @brief  LED1控制
 * @param  state: 0=灭(高电平), 1=亮(低电平)
 * @note   LED为低电平点亮
 * @retval 无
 */
void BSP_LED_Set(uint8_t state);

/**
 * @brief  LED1翻转
 * @retval 无
 */
void BSP_LED_Toggle(void);

/**
 * @brief  读取ADC数据就绪状态
 * @retval 1=数据就绪(低电平), 0=未就绪(高电平)
 */
uint8_t BSP_ADC_IsDataReady(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_GPIO_H */
