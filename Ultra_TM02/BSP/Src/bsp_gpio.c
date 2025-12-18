/**
 * @file    bsp_gpio.c
 * @brief   GPIO板级支持包源文件
 * @details 实现ADC/DAC片选、LOAD信号、LED控制等GPIO操作
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_gpio.h"

/* 私有变量 ------------------------------------------------------------------*/

/* 私有函数 ------------------------------------------------------------------*/

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @brief  GPIO初始化
 * @note   CubeIDE已自动初始化GPIO，此函数设置默认输出状态
 * @retval 无
 */
void BSP_GPIO_Init(void)
{
    /* 所有片选信号设为高电平（不选中） */
    HAL_GPIO_WritePin(ADC1_CS_GPIO_Port, ADC1_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DAC1_CS_GPIO_Port, DAC1_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DAC2_CS_GPIO_Port, DAC2_CS_Pin, GPIO_PIN_SET);
    
    /* 所有LOAD信号设为高电平（正常状态） */
    HAL_GPIO_WritePin(DAC1_LOAD_GPIO_Port, DAC1_LOAD_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DAC2_LOAD_GPIO_Port, DAC2_LOAD_Pin, GPIO_PIN_SET);
    
    /* LED1熄灭（高电平） */
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
}

/**
 * @brief  设置ADC1片选信号
 * @param  state: 0=选中(低电平), 1=取消选中(高电平)
 * @retval 无
 */
void BSP_ADC_CS(uint8_t state)
{
    if (state)
    {
        /* 取消选中 - 高电平 */
        HAL_GPIO_WritePin(ADC1_CS_GPIO_Port, ADC1_CS_Pin, GPIO_PIN_SET);
    }
    else
    {
        /* 选中 - 低电平 */
        HAL_GPIO_WritePin(ADC1_CS_GPIO_Port, ADC1_CS_Pin, GPIO_PIN_RESET);
    }
}

/**
 * @brief  设置DAC1片选信号
 * @param  state: 0=选中(低电平), 1=取消选中(高电平)
 * @retval 无
 */
void BSP_DAC1_CS(uint8_t state)
{
    if (state)
    {
        /* 取消选中 - 高电平 */
        HAL_GPIO_WritePin(DAC1_CS_GPIO_Port, DAC1_CS_Pin, GPIO_PIN_SET);
    }
    else
    {
        /* 选中 - 低电平 */
        HAL_GPIO_WritePin(DAC1_CS_GPIO_Port, DAC1_CS_Pin, GPIO_PIN_RESET);
    }
}

/**
 * @brief  设置DAC2片选信号
 * @param  state: 0=选中(低电平), 1=取消选中(高电平)
 * @retval 无
 */
void BSP_DAC2_CS(uint8_t state)
{
    if (state)
    {
        /* 取消选中 - 高电平 */
        HAL_GPIO_WritePin(DAC2_CS_GPIO_Port, DAC2_CS_Pin, GPIO_PIN_SET);
    }
    else
    {
        /* 选中 - 低电平 */
        HAL_GPIO_WritePin(DAC2_CS_GPIO_Port, DAC2_CS_Pin, GPIO_PIN_RESET);
    }
}

/**
 * @brief  设置DAC1加载信号
 * @param  state: 0=低电平(触发加载), 1=高电平(正常)
 * @retval 无
 */
void BSP_DAC1_LOAD(uint8_t state)
{
    if (state)
    {
        /* 正常状态 - 高电平 */
        HAL_GPIO_WritePin(DAC1_LOAD_GPIO_Port, DAC1_LOAD_Pin, GPIO_PIN_SET);
    }
    else
    {
        /* 触发加载 - 低电平 */
        HAL_GPIO_WritePin(DAC1_LOAD_GPIO_Port, DAC1_LOAD_Pin, GPIO_PIN_RESET);
    }
}

/**
 * @brief  设置DAC2加载信号
 * @param  state: 0=低电平(触发加载), 1=高电平(正常)
 * @retval 无
 */
void BSP_DAC2_LOAD(uint8_t state)
{
    if (state)
    {
        /* 正常状态 - 高电平 */
        HAL_GPIO_WritePin(DAC2_LOAD_GPIO_Port, DAC2_LOAD_Pin, GPIO_PIN_SET);
    }
    else
    {
        /* 触发加载 - 低电平 */
        HAL_GPIO_WritePin(DAC2_LOAD_GPIO_Port, DAC2_LOAD_Pin, GPIO_PIN_RESET);
    }
}

/**
 * @brief  LED1控制
 * @param  state: 0=灭(高电平), 1=亮(低电平)
 * @note   LED为低电平点亮
 * @retval 无
 */
void BSP_LED_Set(uint8_t state)
{
    if (state)
    {
        /* 点亮LED - 低电平 */
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
    }
    else
    {
        /* 熄灭LED - 高电平 */
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
    }
}

/**
 * @brief  LED1翻转
 * @retval 无
 */
void BSP_LED_Toggle(void)
{
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
}

/**
 * @brief  读取ADC数据就绪状态
 * @retval 1=数据就绪(DRDY为低电平), 0=未就绪(DRDY为高电平)
 */
uint8_t BSP_ADC_IsDataReady(void)
{
    /* DRDY低电平表示数据就绪 */
    if (HAL_GPIO_ReadPin(ADC_DRDY_GPIO_Port, ADC_DRDY_Pin) == GPIO_PIN_RESET)
    {
        return 1;  /* 数据就绪 */
    }
    return 0;  /* 未就绪 */
}
