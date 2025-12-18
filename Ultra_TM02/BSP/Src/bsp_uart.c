/**
 * @file    bsp_uart.c
 * @brief   UART板级支持包源文件
 * @details 实现USART6通讯函数，用于LCD串口屏通讯
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_uart.h"
#include <stdio.h>
#include <stdarg.h>

/* 私有变量 ------------------------------------------------------------------*/

/* 环形接收缓冲区 */
static uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint16_t rx_head = 0;  /* 写入位置 */
static volatile uint16_t rx_tail = 0;  /* 读取位置 */

/* 单字节接收缓冲（用于中断接收） */
static uint8_t rx_byte = 0;

/* 私有函数 ------------------------------------------------------------------*/

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @brief  UART初始化
 * @note   CubeIDE已在MX_USART6_UART_Init()中完成初始化
 *         此函数启用接收中断
 * @retval 无
 */
void BSP_UART_Init(void)
{
    /* USART6配置参数（由CubeIDE生成）：
     * - 波特率: 115200
     * - 数据位: 8
     * - 停止位: 1
     * - 校验: None
     * - 硬件流控: None
     */
    
    /* 清空接收缓冲区 */
    rx_head = 0;
    rx_tail = 0;
    
    /* 启用接收中断 */
    HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
}

/**
 * @brief  UART发送数据
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval HAL状态
 */
HAL_StatusTypeDef BSP_UART_Transmit(uint8_t *data, uint16_t len)
{
    return HAL_UART_Transmit(&huart6, data, len, UART_TX_TIMEOUT);
}

/**
 * @brief  UART发送字符串
 * @param  str: 字符串指针
 * @retval 无
 */
void BSP_UART_SendString(const char *str)
{
    uint16_t len = strlen(str);
    HAL_UART_Transmit(&huart6, (uint8_t *)str, len, UART_TX_TIMEOUT);
}

/**
 * @brief  UART发送格式化字符串
 * @param  fmt: 格式化字符串
 * @param  ...: 可变参数
 * @retval 无
 */
void BSP_UART_Printf(const char *fmt, ...)
{
    char buf[256];
    va_list args;
    
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    
    BSP_UART_SendString(buf);
}

/**
 * @brief  检查接收缓冲区是否有数据
 * @retval 可用数据字节数
 */
uint16_t BSP_UART_Available(void)
{
    return (rx_head - rx_tail + UART_RX_BUFFER_SIZE) % UART_RX_BUFFER_SIZE;
}

/**
 * @brief  读取一个字节
 * @retval 数据 (0-255), -1表示无数据
 */
int BSP_UART_Read(void)
{
    /* 检查是否有数据 */
    if (rx_head == rx_tail)
    {
        return -1;  /* 无数据 */
    }
    
    /* 读取数据 */
    uint8_t data = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % UART_RX_BUFFER_SIZE;
    
    return data;
}

/**
 * @brief  读取多个字节
 * @param  data: 数据缓冲区指针
 * @param  max_len: 最大读取长度
 * @retval 实际读取的字节数
 */
uint16_t BSP_UART_ReadBuffer(uint8_t *data, uint16_t max_len)
{
    uint16_t count = 0;
    int byte;
    
    while (count < max_len)
    {
        byte = BSP_UART_Read();
        if (byte < 0)
        {
            break;  /* 无更多数据 */
        }
        data[count++] = (uint8_t)byte;
    }
    
    return count;
}

/**
 * @brief  清空接收缓冲区
 * @retval 无
 */
void BSP_UART_FlushRxBuffer(void)
{
    rx_head = 0;
    rx_tail = 0;
}

/**
 * @brief  UART接收中断回调（在中断处理函数中调用）
 * @param  data: 接收到的数据
 * @retval 无
 */
void BSP_UART_RxCallback(uint8_t data)
{
    /* 计算下一个写入位置 */
    uint16_t next_head = (rx_head + 1) % UART_RX_BUFFER_SIZE;
    
    /* 检查缓冲区是否已满 */
    if (next_head != rx_tail)
    {
        /* 写入数据 */
        rx_buffer[rx_head] = data;
        rx_head = next_head;
    }
    /* 如果缓冲区已满，丢弃数据 */
}

/**
 * @brief  HAL UART接收完成回调函数
 * @note   此函数由HAL库调用，需要在stm32f4xx_it.c中启用
 * @param  huart: UART句柄指针
 * @retval 无
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART6)
    {
        /* 将接收到的字节存入缓冲区 */
        BSP_UART_RxCallback(rx_byte);
        
        /* 继续接收下一个字节 */
        HAL_UART_Receive_IT(&huart6, &rx_byte, 1);
    }
}
