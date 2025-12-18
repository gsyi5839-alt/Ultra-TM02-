/**
 * @file    svc_usb.h
 * @brief   USB服务层头文件
 * @details 提供USB CDC虚拟串口通讯服务
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __SVC_USB_H
#define __SVC_USB_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 宏定义 --------------------------------------------------------------------*/

/* USB接收缓冲区大小 */
#define USB_RX_BUFFER_SIZE      512

/* USB发送超时时间 (ms) */
#define USB_TX_TIMEOUT          100

/* 类型定义 ------------------------------------------------------------------*/

/* USB连接状态 */
typedef enum {
    USB_STATE_DISCONNECTED = 0, /* 未连接 */
    USB_STATE_CONNECTED,        /* 已连接 */
    USB_STATE_CONFIGURED        /* 已配置 */
} USBState_t;

/* 函数声明 ------------------------------------------------------------------*/

/**
 * @brief  USB服务初始化
 * @retval 无
 */
void SVC_USB_Init(void);

/**
 * @brief  获取USB连接状态
 * @retval USB状态
 */
USBState_t SVC_USB_GetState(void);

/**
 * @brief  检查USB是否已连接并配置
 * @retval 1=就绪, 0=未就绪
 */
uint8_t SVC_USB_IsReady(void);

/**
 * @brief  发送数据
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval 0=成功, 非0=失败
 */
uint8_t SVC_USB_Transmit(uint8_t *data, uint16_t len);

/**
 * @brief  检查接收缓冲区是否有数据
 * @retval 可用数据字节数
 */
uint16_t SVC_USB_Available(void);

/**
 * @brief  读取一个字节
 * @retval 数据 (0-255), -1表示无数据
 */
int SVC_USB_ReadByte(void);

/**
 * @brief  读取多个字节
 * @param  data: 数据缓冲区指针
 * @param  max_len: 最大读取长度
 * @retval 实际读取的字节数
 */
uint16_t SVC_USB_ReadBuffer(uint8_t *data, uint16_t max_len);

/**
 * @brief  清空接收缓冲区
 * @retval 无
 */
void SVC_USB_FlushRxBuffer(void);

/**
 * @brief  USB接收回调（由USB中间件调用）
 * @param  data: 接收数据指针
 * @param  len: 数据长度
 * @retval 无
 */
void SVC_USB_RxCallback(uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __SVC_USB_H */
