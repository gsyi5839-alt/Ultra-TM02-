/**
 * @file    bsp_spi.h
 * @brief   SPI板级支持包头文件
 * @details 提供SPI1通讯基础函数，用于ADC/DAC芯片通讯
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 宏定义 --------------------------------------------------------------------*/

/* SPI超时时间 (ms) */
#define BSP_SPI_TIMEOUT     100

/* 类型定义 ------------------------------------------------------------------*/

/* 外部变量 ------------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;

/* 函数声明 ------------------------------------------------------------------*/

/**
 * @brief  SPI1初始化
 * @note   CubeIDE已自动初始化，此函数可用于重新配置
 * @retval 无
 */
void BSP_SPI_Init(void);

/**
 * @brief  SPI收发一个字节
 * @param  tx_data: 要发送的数据
 * @retval 接收到的数据
 */
uint8_t BSP_SPI_TransmitReceive(uint8_t tx_data);

/**
 * @brief  SPI发送多个字节
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval HAL状态
 */
HAL_StatusTypeDef BSP_SPI_Transmit(uint8_t *data, uint16_t len);

/**
 * @brief  SPI接收多个字节
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval HAL状态
 */
HAL_StatusTypeDef BSP_SPI_Receive(uint8_t *data, uint16_t len);

/**
 * @brief  SPI同时收发多个字节
 * @param  tx_data: 发送数据缓冲区指针
 * @param  rx_data: 接收数据缓冲区指针
 * @param  len: 数据长度
 * @retval HAL状态
 */
HAL_StatusTypeDef BSP_SPI_TransmitReceiveBuffer(uint8_t *tx_data, uint8_t *rx_data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_SPI_H */
