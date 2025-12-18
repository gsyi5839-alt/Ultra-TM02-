/**
 * @file    stm32f4xx_it.h
 * @brief   中断服务程序头文件
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __STM32F4XX_IT_H
#define __STM32F4XX_IT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 中断服务函数声明 ----------------------------------------------------------*/

/* Cortex-M4处理器异常 */
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/* 外设中断 */
void EXTI0_IRQHandler(void);        /* ADC_DRDY中断 */
void SPI1_IRQHandler(void);         /* SPI1中断 */
void USART6_IRQHandler(void);       /* USART6中断 (LCD) */
void OTG_FS_IRQHandler(void);       /* USB OTG FS中断 */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4XX_IT_H */

