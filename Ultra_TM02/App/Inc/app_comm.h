/**
 * @file    app_comm.h
 * @brief   通讯处理应用层头文件
 * @details 提供USB CDC协议解析和命令处理功能
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __APP_COMM_H
#define __APP_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 宏定义 --------------------------------------------------------------------*/

/* 帧定义 */
#define FRAME_HEAD              0xAA        /* 帧头 */
#define FRAME_TAIL              0x55        /* 帧尾 */
#define MAX_DATA_LEN            256         /* 最大数据长度 */

/* 命令码定义 */
#define CMD_GET_DEVICE_ID       0x01        /* 获取设备ID */
#define CMD_GET_TEMPERATURE     0x02        /* 获取温度值 */
#define CMD_GET_VOLTAGE         0x03        /* 获取电压值 */
#define CMD_GET_CURRENT         0x04        /* 获取输出电流 */
#define CMD_GET_STATUS          0x05        /* 获取设备状态 */
#define CMD_SET_CURRENT_SRC     0x10        /* 设置电流源 */
#define CMD_SET_CURRENT_ADJ_10  0x11        /* 设置10μA调整值 */
#define CMD_SET_CURRENT_ADJ_17  0x12        /* 设置17μA调整值 */
#define CMD_SET_4MA_TEMP        0x20        /* 设置4mA温度点 */
#define CMD_SET_20MA_TEMP       0x21        /* 设置20mA温度点 */
#define CMD_START_ACQ           0x30        /* 开始采集 */
#define CMD_STOP_ACQ            0x31        /* 停止采集 */
#define CMD_LOAD_TABLE_START    0x40        /* 分度表下载开始 */
#define CMD_LOAD_TABLE_DATA     0x41        /* 分度表数据包 */
#define CMD_LOAD_TABLE_END      0x42        /* 分度表下载结束 */
#define CMD_SAVE_PARAM          0x50        /* 保存参数 */
#define CMD_LOAD_PARAM          0x51        /* 加载参数 */
#define CMD_RESET_DEFAULT       0x52        /* 恢复默认 */
#define CMD_ACK                 0x80        /* 确认响应 */
#define CMD_NACK                0x81        /* 否定响应 */
#define CMD_DATA_REPORT         0xF0        /* 数据主动上报 */

/* 状态码定义 */
#define STATUS_OK               0x00        /* 成功 */
#define STATUS_INVALID_CMD      0x01        /* 命令无效 */
#define STATUS_INVALID_PARAM    0x02        /* 参数错误 */
#define STATUS_CRC_ERROR        0x03        /* CRC错误 */
#define STATUS_BUSY             0x04        /* 忙 */
#define STATUS_FLASH_ERROR      0x05        /* Flash写入失败 */
#define STATUS_TABLE_ERROR      0x06        /* 分度表错误 */

/* 设备ID长度 */
#define DEVICE_ID_LEN           16

/* 类型定义 ------------------------------------------------------------------*/

/* 帧结构 */
typedef struct {
    uint8_t head;               /* 帧头 */
    uint8_t cmd;                /* 命令码 */
    uint8_t len;                /* 数据长度 */
    uint8_t data[MAX_DATA_LEN]; /* 数据 */
    uint16_t crc;               /* CRC校验 */
    uint8_t tail;               /* 帧尾 */
} Frame_t;

/* 解析状态 */
typedef enum {
    PARSE_HEAD = 0,
    PARSE_CMD,
    PARSE_LEN,
    PARSE_DATA,
    PARSE_CRC_L,
    PARSE_CRC_H,
    PARSE_TAIL
} ParseState_t;

/* 函数声明 ------------------------------------------------------------------*/

/**
 * @brief  通讯模块初始化
 * @retval 无
 */
void APP_Comm_Init(void);

/**
 * @brief  通讯处理（主循环中调用）
 * @retval 无
 */
void APP_Comm_Process(void);

/**
 * @brief  发送确认响应
 * @param  cmd: 原命令
 * @param  status: 状态码
 * @retval 无
 */
void APP_Comm_SendAck(uint8_t cmd, uint8_t status);

/**
 * @brief  发送数据响应
 * @param  cmd: 命令码
 * @param  data: 数据指针
 * @param  len: 数据长度
 * @retval 无
 */
void APP_Comm_SendData(uint8_t cmd, uint8_t *data, uint8_t len);

/**
 * @brief  主动上报数据
 * @param  temp: 温度值 (℃)
 * @param  voltage: 电压值 (mV)
 * @param  current: 输出电流 (mA)
 * @retval 无
 */
void APP_Comm_ReportData(float temp, float voltage, float current);

/**
 * @brief  CRC16计算
 * @param  data: 数据指针
 * @param  len: 数据长度
 * @retval CRC16值
 */
uint16_t APP_Comm_CRC16(uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __APP_COMM_H */
