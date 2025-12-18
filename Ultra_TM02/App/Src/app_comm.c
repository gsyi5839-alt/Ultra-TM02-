/**
 * @file    app_comm.c
 * @brief   通讯处理应用层源文件
 * @details 实现USB CDC协议解析和命令处理功能
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "app_comm.h"
#include "app_temp.h"
#include "app_output.h"
#include "app_param.h"
#include "svc_usb.h"
#include "svc_dac.h"
#include <string.h>

/* 私有变量 ------------------------------------------------------------------*/

/* 解析状态 */
static ParseState_t parse_state = PARSE_HEAD;

/* 接收帧缓冲 */
static Frame_t rx_frame;
static uint16_t data_index = 0;

/* 设备ID */
static const char device_id[DEVICE_ID_LEN] = "TM02-00000001";

/* 私有函数声明 --------------------------------------------------------------*/
static void ProcessFrame(Frame_t *frame);
static void ParseByte(uint8_t byte);

/* 私有函数 ------------------------------------------------------------------*/

/**
 * @brief  处理接收到的帧
 * @param  frame: 帧指针
 * @retval 无
 */
static void ProcessFrame(Frame_t *frame)
{
    uint8_t data[16];
    float fval;
    
    switch (frame->cmd)
    {
        /* 获取设备ID */
        case CMD_GET_DEVICE_ID:
            memcpy(data, device_id, DEVICE_ID_LEN);
            APP_Comm_SendData(CMD_GET_DEVICE_ID, data, DEVICE_ID_LEN);
            break;
            
        /* 获取温度值 */
        case CMD_GET_TEMPERATURE:
            fval = APP_Temp_GetValue();
            APP_Comm_SendData(CMD_GET_TEMPERATURE, (uint8_t *)&fval, 4);
            break;
            
        /* 获取电压值 */
        case CMD_GET_VOLTAGE:
            fval = APP_Temp_GetVoltage();
            APP_Comm_SendData(CMD_GET_VOLTAGE, (uint8_t *)&fval, 4);
            break;
            
        /* 获取输出电流 */
        case CMD_GET_CURRENT:
            fval = APP_Output_GetCurrent();
            APP_Comm_SendData(CMD_GET_CURRENT, (uint8_t *)&fval, 4);
            break;
            
        /* 获取设备状态 */
        case CMD_GET_STATUS:
            {
                uint8_t status_data[8];
                status_data[0] = APP_Temp_IsRunning();
                status_data[1] = APP_Temp_GetCurrentSource();
                status_data[2] = (uint8_t)APP_Temp_GetProbeStatus();
                status_data[3] = 0;  /* 保留 */
                uint32_t count = APP_Temp_GetSampleCount();
                memcpy(&status_data[4], &count, 4);
                APP_Comm_SendData(CMD_GET_STATUS, status_data, 8);
            }
            break;
            
        /* 设置电流源 */
        case CMD_SET_CURRENT_SRC:
            if (frame->len >= 1 && frame->data[0] <= 1)
            {
                APP_Temp_SetCurrentSource(frame->data[0]);
                APP_Param_SetCurrentSource(frame->data[0]);
                APP_Comm_SendAck(frame->cmd, STATUS_OK);
            }
            else
            {
                APP_Comm_SendAck(frame->cmd, STATUS_INVALID_PARAM);
            }
            break;
            
        /* 设置10μA调整值 */
        case CMD_SET_CURRENT_ADJ_10:
            if (frame->len >= 4)
            {
                memcpy(&fval, frame->data, 4);
                APP_Param_SetCurrentAdj10(fval);
                SVC_DAC_SetCurrentAdj(CURRENT_SRC_10UA, fval);
                APP_Comm_SendAck(frame->cmd, STATUS_OK);
            }
            else
            {
                APP_Comm_SendAck(frame->cmd, STATUS_INVALID_PARAM);
            }
            break;
            
        /* 设置17μA调整值 */
        case CMD_SET_CURRENT_ADJ_17:
            if (frame->len >= 4)
            {
                memcpy(&fval, frame->data, 4);
                APP_Param_SetCurrentAdj17(fval);
                SVC_DAC_SetCurrentAdj(CURRENT_SRC_17UA, fval);
                APP_Comm_SendAck(frame->cmd, STATUS_OK);
            }
            else
            {
                APP_Comm_SendAck(frame->cmd, STATUS_INVALID_PARAM);
            }
            break;
            
        /* 设置4mA温度点 */
        case CMD_SET_4MA_TEMP:
            if (frame->len >= 4)
            {
                memcpy(&fval, frame->data, 4);
                APP_Param_Set4mATemp(fval);
                APP_Output_Set4mATemp(fval);
                APP_Comm_SendAck(frame->cmd, STATUS_OK);
            }
            else
            {
                APP_Comm_SendAck(frame->cmd, STATUS_INVALID_PARAM);
            }
            break;
            
        /* 设置20mA温度点 */
        case CMD_SET_20MA_TEMP:
            if (frame->len >= 4)
            {
                memcpy(&fval, frame->data, 4);
                APP_Param_Set20mATemp(fval);
                APP_Output_Set20mATemp(fval);
                APP_Comm_SendAck(frame->cmd, STATUS_OK);
            }
            else
            {
                APP_Comm_SendAck(frame->cmd, STATUS_INVALID_PARAM);
            }
            break;
            
        /* 开始采集 */
        case CMD_START_ACQ:
            APP_Temp_Start();
            APP_Comm_SendAck(frame->cmd, STATUS_OK);
            break;
            
        /* 停止采集 */
        case CMD_STOP_ACQ:
            APP_Temp_Stop();
            APP_Comm_SendAck(frame->cmd, STATUS_OK);
            break;
            
        /* 保存参数 */
        case CMD_SAVE_PARAM:
            if (APP_Param_Save() == 0)
            {
                APP_Comm_SendAck(frame->cmd, STATUS_OK);
            }
            else
            {
                APP_Comm_SendAck(frame->cmd, STATUS_FLASH_ERROR);
            }
            break;
            
        /* 加载参数 */
        case CMD_LOAD_PARAM:
            if (APP_Param_Load() == 0)
            {
                APP_Comm_SendAck(frame->cmd, STATUS_OK);
            }
            else
            {
                APP_Comm_SendAck(frame->cmd, STATUS_FLASH_ERROR);
            }
            break;
            
        /* 恢复默认 */
        case CMD_RESET_DEFAULT:
            APP_Param_SetDefault();
            APP_Comm_SendAck(frame->cmd, STATUS_OK);
            break;
            
        /* 未知命令 */
        default:
            APP_Comm_SendAck(frame->cmd, STATUS_INVALID_CMD);
            break;
    }
}

/**
 * @brief  解析一个字节
 * @param  byte: 接收到的字节
 * @retval 无
 */
static void ParseByte(uint8_t byte)
{
    switch (parse_state)
    {
        case PARSE_HEAD:
            if (byte == FRAME_HEAD)
            {
                rx_frame.head = byte;
                parse_state = PARSE_CMD;
            }
            break;
            
        case PARSE_CMD:
            rx_frame.cmd = byte;
            parse_state = PARSE_LEN;
            break;
            
        case PARSE_LEN:
            rx_frame.len = byte;
            data_index = 0;
            if (byte > 0)
            {
                parse_state = PARSE_DATA;
            }
            else
            {
                parse_state = PARSE_CRC_L;
            }
            break;
            
        case PARSE_DATA:
            rx_frame.data[data_index++] = byte;
            if (data_index >= rx_frame.len)
            {
                parse_state = PARSE_CRC_L;
            }
            break;
            
        case PARSE_CRC_L:
            rx_frame.crc = byte;
            parse_state = PARSE_CRC_H;
            break;
            
        case PARSE_CRC_H:
            rx_frame.crc |= ((uint16_t)byte << 8);
            parse_state = PARSE_TAIL;
            break;
            
        case PARSE_TAIL:
            if (byte == FRAME_TAIL)
            {
                /* 验证CRC */
                uint8_t crc_buf[MAX_DATA_LEN + 2];
                crc_buf[0] = rx_frame.cmd;
                crc_buf[1] = rx_frame.len;
                memcpy(&crc_buf[2], rx_frame.data, rx_frame.len);
                
                uint16_t calc_crc = APP_Comm_CRC16(crc_buf, rx_frame.len + 2);
                
                if (calc_crc == rx_frame.crc)
                {
                    /* CRC正确，处理帧 */
                    ProcessFrame(&rx_frame);
                }
                else
                {
                    /* CRC错误 */
                    APP_Comm_SendAck(rx_frame.cmd, STATUS_CRC_ERROR);
                }
            }
            parse_state = PARSE_HEAD;
            break;
    }
}

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @brief  通讯模块初始化
 * @retval 无
 */
void APP_Comm_Init(void)
{
    parse_state = PARSE_HEAD;
    data_index = 0;
    
    /* 初始化USB服务 */
    SVC_USB_Init();
}

/**
 * @brief  通讯处理（主循环中调用）
 * @retval 无
 */
void APP_Comm_Process(void)
{
    int byte;
    
    /* 处理所有接收到的字节 */
    while ((byte = SVC_USB_ReadByte()) >= 0)
    {
        ParseByte((uint8_t)byte);
    }
}

/**
 * @brief  发送确认响应
 * @param  cmd: 原命令
 * @param  status: 状态码
 * @retval 无
 */
void APP_Comm_SendAck(uint8_t cmd, uint8_t status)
{
    uint8_t buf[8];
    uint16_t crc;
    
    buf[0] = FRAME_HEAD;
    buf[1] = CMD_ACK;
    buf[2] = 1;         /* 数据长度 */
    buf[3] = status;    /* 状态码 */
    
    /* 计算CRC (命令+长度+数据) */
    crc = APP_Comm_CRC16(&buf[1], 3);
    buf[4] = crc & 0xFF;
    buf[5] = (crc >> 8) & 0xFF;
    buf[6] = FRAME_TAIL;
    
    /* 发送 */
    SVC_USB_Transmit(buf, 7);
}

/**
 * @brief  发送数据响应
 * @param  cmd: 命令码
 * @param  data: 数据指针
 * @param  len: 数据长度
 * @retval 无
 */
void APP_Comm_SendData(uint8_t cmd, uint8_t *data, uint8_t len)
{
    uint8_t buf[MAX_DATA_LEN + 6];
    uint16_t crc;
    uint16_t idx = 0;
    
    buf[idx++] = FRAME_HEAD;
    buf[idx++] = cmd;
    buf[idx++] = len;
    
    /* 复制数据 */
    memcpy(&buf[idx], data, len);
    idx += len;
    
    /* 计算CRC */
    crc = APP_Comm_CRC16(&buf[1], len + 2);
    buf[idx++] = crc & 0xFF;
    buf[idx++] = (crc >> 8) & 0xFF;
    buf[idx++] = FRAME_TAIL;
    
    /* 发送 */
    SVC_USB_Transmit(buf, idx);
}

/**
 * @brief  主动上报数据
 * @param  temp: 温度值 (℃)
 * @param  voltage: 电压值 (mV)
 * @param  current: 输出电流 (mA)
 * @retval 无
 */
void APP_Comm_ReportData(float temp, float voltage, float current)
{
    uint8_t data[12];
    
    memcpy(&data[0], &temp, 4);
    memcpy(&data[4], &voltage, 4);
    memcpy(&data[8], &current, 4);
    
    APP_Comm_SendData(CMD_DATA_REPORT, data, 12);
}

/**
 * @brief  CRC16计算 (CCITT/Modbus)
 * @param  data: 数据指针
 * @param  len: 数据长度
 * @retval CRC16值
 */
uint16_t APP_Comm_CRC16(uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    
    while (len--)
    {
        crc ^= *data++;
        for (int i = 0; i < 8; i++)
        {
            if (crc & 0x0001)
            {
                crc = (crc >> 1) ^ 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}
