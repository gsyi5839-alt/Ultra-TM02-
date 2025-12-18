/**
 * @file    stm32f4xx_it.c
 * @brief   中断服务程序实现文件
 * @version V1.0
 * @date    2025-12-18
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"

/* External variables --------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart6;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* ADC服务回调声明 */
extern void SVC_ADC_DRDY_Callback(void);

/******************************************************************************/
/*           Cortex-M4 处理器异常处理程序                                       */
/******************************************************************************/

/**
 * @brief  NMI中断处理
 */
void NMI_Handler(void)
{
    while (1)
    {
    }
}

/**
 * @brief  硬件错误中断处理
 */
void HardFault_Handler(void)
{
    while (1)
    {
    }
}

/**
 * @brief  内存管理异常处理
 */
void MemManage_Handler(void)
{
    while (1)
    {
    }
}

/**
 * @brief  总线错误异常处理
 */
void BusFault_Handler(void)
{
    while (1)
    {
    }
}

/**
 * @brief  用法错误异常处理
 */
void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

/**
 * @brief  SVC中断处理
 */
void SVC_Handler(void)
{
}

/**
 * @brief  调试监视中断处理
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  PendSV中断处理
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  SysTick中断处理
 * @note   每1ms调用一次，为HAL库提供时基
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/******************************************************************************/
/*           外设中断处理程序                                                   */
/******************************************************************************/

/**
 * @brief  EXTI0中断处理 (ADC_DRDY)
 * @note   ADC数据就绪时触发
 */
void EXTI0_IRQHandler(void)
{
    /* 清除中断标志 */
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
 * @brief  GPIO外部中断回调
 * @param  GPIO_Pin: 触发中断的引脚
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0)
    {
        /* ADC数据就绪回调 */
        SVC_ADC_DRDY_Callback();
    }
}

/**
 * @brief  SPI1中断处理
 */
void SPI1_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi1);
}

/**
 * @brief  USART6中断处理 (LCD串口屏)
 */
void USART6_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart6);
}

/**
 * @brief  USB OTG FS中断处理
 */
void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

