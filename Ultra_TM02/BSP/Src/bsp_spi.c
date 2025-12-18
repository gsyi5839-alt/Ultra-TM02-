/**
 * @file    bsp_spi.c
 * @brief   SPI板级支持包源文件
 * @details 实现SPI1通讯基础函数，用于ADC/DAC芯片通讯
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_spi.h"

/* 私有变量 ------------------------------------------------------------------*/

/* 私有函数 ------------------------------------------------------------------*/

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @brief  SPI1初始化
 * @note   CubeIDE已在MX_SPI1_Init()中完成初始化
 *         此函数可用于额外配置或状态检查
 * @retval 无
 */
void BSP_SPI_Init(void)
{
    /* SPI1配置参数（由CubeIDE生成）：
     * - 模式: Full-Duplex Master
     * - 数据大小: 8 bits
     * - 时钟极性: High (CPOL=1)
     * - 时钟相位: 2 Edge (CPHA=1)
     * - NSS: Software
     * - 波特率预分频: 8 (PCLK2/8 = 100MHz/8 = 12.5MHz)
     * - 首位: MSB First
     */
    
    /* 确保SPI已启用 */
    if (hspi1.State == HAL_SPI_STATE_RESET)
    {
        /* 如果未初始化，调用HAL初始化 */
        /* 注意：正常流程下CubeIDE生成的代码会自动初始化 */
    }
}

/**
 * @brief  SPI收发一个字节
 * @param  tx_data: 要发送的数据
 * @retval 接收到的数据
 */
uint8_t BSP_SPI_TransmitReceive(uint8_t tx_data)
{
    uint8_t rx_data = 0;
    
    /* 同时发送和接收一个字节 */
    HAL_SPI_TransmitReceive(&hspi1, &tx_data, &rx_data, 1, BSP_SPI_TIMEOUT);
    
    return rx_data;
}

/**
 * @brief  SPI发送多个字节
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval HAL状态
 */
HAL_StatusTypeDef BSP_SPI_Transmit(uint8_t *data, uint16_t len)
{
    return HAL_SPI_Transmit(&hspi1, data, len, BSP_SPI_TIMEOUT);
}

/**
 * @brief  SPI接收多个字节
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval HAL状态
 */
HAL_StatusTypeDef BSP_SPI_Receive(uint8_t *data, uint16_t len)
{
    return HAL_SPI_Receive(&hspi1, data, len, BSP_SPI_TIMEOUT);
}

/**
 * @brief  SPI同时收发多个字节
 * @param  tx_data: 发送数据缓冲区指针
 * @param  rx_data: 接收数据缓冲区指针
 * @param  len: 数据长度
 * @retval HAL状态
 */
HAL_StatusTypeDef BSP_SPI_TransmitReceiveBuffer(uint8_t *tx_data, uint8_t *rx_data, uint16_t len)
{
    return HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, len, BSP_SPI_TIMEOUT);
}
