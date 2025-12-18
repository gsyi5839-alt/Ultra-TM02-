/**
 * @file    svc_usb.c
 * @brief   USB服务层源文件
 * @details 实现USB CDC虚拟串口通讯服务
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "svc_usb.h"
#include "usbd_cdc_if.h"

/* 私有变量 ------------------------------------------------------------------*/

/* 环形接收缓冲区 */
static uint8_t rx_buffer[USB_RX_BUFFER_SIZE];
static volatile uint16_t rx_head = 0;  /* 写入位置 */
static volatile uint16_t rx_tail = 0;  /* 读取位置 */

/* USB状态 */
static USBState_t usb_state = USB_STATE_DISCONNECTED;

/* 私有函数 ------------------------------------------------------------------*/

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @brief  USB服务初始化
 * @note   USB已由CubeIDE生成的代码初始化
 * @retval 无
 */
void SVC_USB_Init(void)
{
    /* 清空接收缓冲区 */
    rx_head = 0;
    rx_tail = 0;
    
    /* USB初始化由MX_USB_DEVICE_Init()完成 */
}

/**
 * @brief  获取USB连接状态
 * @retval USB状态
 */
USBState_t SVC_USB_GetState(void)
{
    /* 可以通过检查USB设备状态来更新 */
    /* 这里简化处理，假设已配置 */
    return usb_state;
}

/**
 * @brief  检查USB是否已连接并配置
 * @retval 1=就绪, 0=未就绪
 */
uint8_t SVC_USB_IsReady(void)
{
    /* 检查USB设备是否已配置 */
    extern USBD_HandleTypeDef hUsbDeviceFS;
    
    if (hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED)
    {
        usb_state = USB_STATE_CONFIGURED;
        return 1;
    }
    
    return 0;
}

/**
 * @brief  发送数据
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval 0=成功, 非0=失败
 */
uint8_t SVC_USB_Transmit(uint8_t *data, uint16_t len)
{
    uint32_t timeout = HAL_GetTick() + USB_TX_TIMEOUT;
    uint8_t result;
    
    /* 检查USB是否就绪 */
    if (!SVC_USB_IsReady())
    {
        return 1;  /* USB未就绪 */
    }
    
    /* 等待上次发送完成 */
    while (CDC_GetTxState() != 0)
    {
        if (HAL_GetTick() > timeout)
        {
            return 2;  /* 超时 */
        }
    }
    
    /* 发送数据 */
    result = CDC_Transmit_FS(data, len);
    
    return result;
}

/**
 * @brief  检查接收缓冲区是否有数据
 * @retval 可用数据字节数
 */
uint16_t SVC_USB_Available(void)
{
    return (rx_head - rx_tail + USB_RX_BUFFER_SIZE) % USB_RX_BUFFER_SIZE;
}

/**
 * @brief  读取一个字节
 * @retval 数据 (0-255), -1表示无数据
 */
int SVC_USB_ReadByte(void)
{
    /* 检查是否有数据 */
    if (rx_head == rx_tail)
    {
        return -1;  /* 无数据 */
    }
    
    /* 读取数据 */
    uint8_t data = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % USB_RX_BUFFER_SIZE;
    
    return data;
}

/**
 * @brief  读取多个字节
 * @param  data: 数据缓冲区指针
 * @param  max_len: 最大读取长度
 * @retval 实际读取的字节数
 */
uint16_t SVC_USB_ReadBuffer(uint8_t *data, uint16_t max_len)
{
    uint16_t count = 0;
    int byte;
    
    while (count < max_len)
    {
        byte = SVC_USB_ReadByte();
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
void SVC_USB_FlushRxBuffer(void)
{
    rx_head = 0;
    rx_tail = 0;
}

/**
 * @brief  USB接收回调（由USB中间件调用）
 * @param  data: 接收数据指针
 * @param  len: 数据长度
 * @note   此函数需要在usbd_cdc_if.c的CDC_Receive_FS中调用
 * @retval 无
 */
void SVC_USB_RxCallback(uint8_t *data, uint32_t len)
{
    uint32_t i;
    uint16_t next_head;
    
    for (i = 0; i < len; i++)
    {
        /* 计算下一个写入位置 */
        next_head = (rx_head + 1) % USB_RX_BUFFER_SIZE;
        
        /* 检查缓冲区是否已满 */
        if (next_head != rx_tail)
        {
            /* 写入数据 */
            rx_buffer[rx_head] = data[i];
            rx_head = next_head;
        }
        /* 如果缓冲区已满，丢弃数据 */
    }
}

/**
 * @brief  获取发送状态（提供给外部使用）
 * @retval 发送状态，需要根据USB中间件实现
 */
__weak uint8_t CDC_GetTxState(void)
{
    /* 弱函数，需要在usbd_cdc_if.c中实现 */
    /* 返回0表示可以发送，非0表示忙 */
    return 0;
}
