/**
 * @file    stm32f4xx_hal_conf.h
 * @brief   HAL库配置头文件
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __STM32F4XX_HAL_CONF_H
#define __STM32F4XX_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* ########################## 模块选择 ##################################### */
/* 选择需要使用的HAL模块 */

#define HAL_MODULE_ENABLED
#define HAL_CORTEX_ENABLED
#define HAL_DMA_ENABLED
#define HAL_FLASH_ENABLED
#define HAL_GPIO_ENABLED
#define HAL_PWR_ENABLED
#define HAL_RCC_ENABLED
#define HAL_SPI_ENABLED
#define HAL_UART_ENABLED
#define HAL_PCD_ENABLED         /* USB设备控制器 */

/* ########################## HSE/HSI 配置 ################################# */

#if !defined(HSE_VALUE)
#define HSE_VALUE               ((uint32_t)12000000U)   /* 外部高速晶振 12MHz */
#endif

#if !defined(HSE_STARTUP_TIMEOUT)
#define HSE_STARTUP_TIMEOUT     ((uint32_t)100U)        /* HSE启动超时 (ms) */
#endif

#if !defined(HSI_VALUE)
#define HSI_VALUE               ((uint32_t)16000000U)   /* 内部高速RC 16MHz */
#endif

#if !defined(LSI_VALUE)
#define LSI_VALUE               ((uint32_t)32000U)      /* 内部低速RC 32kHz */
#endif

#if !defined(LSE_VALUE)
#define LSE_VALUE               ((uint32_t)32768U)      /* 外部低速晶振 32.768kHz */
#endif

#if !defined(LSE_STARTUP_TIMEOUT)
#define LSE_STARTUP_TIMEOUT     ((uint32_t)5000U)       /* LSE启动超时 (ms) */
#endif

#if !defined(EXTERNAL_CLOCK_VALUE)
#define EXTERNAL_CLOCK_VALUE    ((uint32_t)12288000U)   /* I2S外部时钟 */
#endif

/* ########################## 系统配置 ##################################### */

#define VDD_VALUE               ((uint32_t)3300U)       /* VDD电压 (mV) */
#define TICK_INT_PRIORITY       ((uint32_t)0U)          /* SysTick中断优先级 */
#define USE_RTOS                0U
#define PREFETCH_ENABLE         1U
#define INSTRUCTION_CACHE_ENABLE 1U
#define DATA_CACHE_ENABLE       1U

/* ########################## 断言选择 ##################################### */

/* 取消注释以启用断言 */
/* #define USE_FULL_ASSERT       1U */

/* ########################## 以太网配置 (未使用) ########################## */

#define MAC_ADDR0               2U
#define MAC_ADDR1               0U
#define MAC_ADDR2               0U
#define MAC_ADDR3               0U
#define MAC_ADDR4               0U
#define MAC_ADDR5               0U

#define ETH_RX_BUF_SIZE         ETH_MAX_PACKET_SIZE
#define ETH_TX_BUF_SIZE         ETH_MAX_PACKET_SIZE
#define ETH_RXBUFNB             4U
#define ETH_TXBUFNB             4U

/* ########################## SPI配置 ###################################### */

#define USE_SPI_CRC             0U

/* ########################## 包含HAL头文件 ################################ */

#ifdef HAL_RCC_MODULE_ENABLED
#include "stm32f4xx_hal_rcc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
#include "stm32f4xx_hal_gpio.h"
#endif

#ifdef HAL_EXTI_MODULE_ENABLED
#include "stm32f4xx_hal_exti.h"
#endif

#ifdef HAL_DMA_MODULE_ENABLED
#include "stm32f4xx_hal_dma.h"
#endif

#ifdef HAL_CORTEX_MODULE_ENABLED
#include "stm32f4xx_hal_cortex.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
#include "stm32f4xx_hal_flash.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED
#include "stm32f4xx_hal_pwr.h"
#endif

#ifdef HAL_SPI_MODULE_ENABLED
#include "stm32f4xx_hal_spi.h"
#endif

#ifdef HAL_UART_MODULE_ENABLED
#include "stm32f4xx_hal_uart.h"
#endif

#ifdef HAL_PCD_MODULE_ENABLED
#include "stm32f4xx_hal_pcd.h"
#endif

/* ########################## 断言宏 ####################################### */

#ifdef USE_FULL_ASSERT
#define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4XX_HAL_CONF_H */

