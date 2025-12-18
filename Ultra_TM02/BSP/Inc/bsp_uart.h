/**
 * @file    bsp_uart.h
 * @brief   UART板级支持包头文件
 * @details 提供USART6通讯函数，用于LCD串口屏通讯
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __BSP_UART_H
#define __BSP_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include <string.h>

/* 宏定义 --------------------------------------------------------------------*/

/* UART接收缓冲区大小 */
#define UART_RX_BUFFER_SIZE     256

/* UART发送超时时间 (ms) */
#define UART_TX_TIMEOUT         1000

/* 类型定义 ------------------------------------------------------------------*/

/* 外部变量 ------------------------------------------------------------------*/
extern UART_HandleTypeDef huart6;

/* 函数声明 ------------------------------------------------------------------*/

/**
 * @brief  UART初始化
 * @note   CubeIDE已自动初始化，此函数启用接收中断
 * @retval 无
 */
void BSP_UART_Init(void);

/**
 * @brief  UART发送数据
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval HAL状态
 */
HAL_StatusTypeDef BSP_UART_Transmit(uint8_t *data, uint16_t len);

/**
 * @brief  UART发送字符串
 * @param  str: 字符串指针
 * @retval 无
 */
void BSP_UART_SendString(const char *str);

/**
 * @brief  UART发送格式化字符串
 * @param  fmt: 格式化字符串
 * @param  ...: 可变参数
 * @retval 无
 */
void BSP_UART_Printf(const char *fmt, ...);

/**
 * @brief  检查接收缓冲区是否有数据
 * @retval 可用数据字节数
 */
uint16_t BSP_UART_Available(void);

/**
 * @brief  读取一个字节
 * @retval 数据 (-1表示无数据)
 */
int BSP_UART_Read(void);

/**
 * @brief  读取多个字节
 * @param  data: 数据缓冲区指针
 * @param  max_len: 最大读取长度
 * @retval 实际读取的字节数
 */
uint16_t BSP_UART_ReadBuffer(uint8_t *data, uint16_t max_len);

/**
 * @brief  清空接收缓冲区
 * @retval 无
 */
void BSP_UART_FlushRxBuffer(void);

/**
 * @brief  UART接收中断回调（在中断处理函数中调用）
 * @param  data: 接收到的数据
 * @retval 无
 */
void BSP_UART_RxCallback(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_UART_H */
