/**
 * @file    app_temp.h
 * @brief   温度测量应用层头文件
 * @details 提供温度测量、滤波、查表等功能
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __APP_TEMP_H
#define __APP_TEMP_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 宏定义 --------------------------------------------------------------------*/

/* 采样缓冲区大小 */
#define TEMP_SAMPLE_COUNT       5

/* 滑动平均滤波器窗口大小 */
#define TEMP_FILTER_SIZE        16

/* 分度表最大点数 */
#define TEMP_TABLE_MAX_POINTS   4871

/* 分度表存储地址 */
#define TEMP_TABLE_FLASH_ADDR   0x08040000

/* 分度表魔数 */
#define TEMP_TABLE_MAGIC        0x004C4254  /* "TBL\0" */

/* 类型定义 ------------------------------------------------------------------*/

/* 测量状态 */
typedef enum {
    TEMP_STATE_IDLE = 0,        /* 空闲 */
    TEMP_STATE_SAMPLING,        /* 采样中 */
    TEMP_STATE_FILTERING,       /* 滤波中 */
    TEMP_STATE_CALCULATING,     /* 计算中 */
    TEMP_STATE_OUTPUTTING,      /* 输出中 */
    TEMP_STATE_ERROR            /* 错误 */
} TempState_t;

/* 探头状态 */
typedef enum {
    PROBE_STATUS_OK = 0,        /* 正常 */
    PROBE_STATUS_OPEN,          /* 断开 */
    PROBE_STATUS_SHORT,         /* 短路 */
    PROBE_STATUS_RANGE_ERR      /* 超量程 */
} ProbeStatus_t;

/* 温度测量数据结构 */
typedef struct {
    TempState_t state;          /* 测量状态 */
    ProbeStatus_t probe_status; /* 探头状态 */
    uint8_t current_src;        /* 电流源选择 (0:10μA, 1:17μA) */
    uint8_t running;            /* 运行标志 */
    float raw_voltage;          /* 原始电压 (mV) */
    float filtered_voltage;     /* 滤波后电压 (mV) */
    float temperature_K;        /* 温度值 (K) */
    float temperature_C;        /* 温度值 (℃) */
    uint32_t sample_count;      /* 采样计数 */
} TempMeasure_t;

/* 分度表数据点 */
typedef struct {
    float voltage;              /* 电压值 (mV) */
    float temperature;          /* 温度值 (K) */
} TempTablePoint_t;

/* 分度表头部 */
typedef struct {
    uint32_t magic;             /* 魔数 */
    uint16_t point_count;       /* 数据点数 */
    uint16_t reserved;          /* 保留 */
} TempTableHeader_t;

/* 函数声明 ------------------------------------------------------------------*/

/**
 * @brief  温度测量初始化
 * @retval 无
 */
void APP_Temp_Init(void);

/**
 * @brief  开始温度测量
 * @retval 无
 */
void APP_Temp_Start(void);

/**
 * @brief  停止温度测量
 * @retval 无
 */
void APP_Temp_Stop(void);

/**
 * @brief  温度测量处理（主循环中调用）
 * @retval 无
 */
void APP_Temp_Process(void);

/**
 * @brief  获取当前温度值
 * @retval 温度值 (℃)
 */
float APP_Temp_GetValue(void);

/**
 * @brief  获取当前温度值（开尔文）
 * @retval 温度值 (K)
 */
float APP_Temp_GetValueK(void);

/**
 * @brief  获取当前电压值
 * @retval 电压值 (mV)
 */
float APP_Temp_GetVoltage(void);

/**
 * @brief  获取探头状态
 * @retval 探头状态
 */
ProbeStatus_t APP_Temp_GetProbeStatus(void);

/**
 * @brief  获取测量状态
 * @retval 测量状态
 */
TempState_t APP_Temp_GetState(void);

/**
 * @brief  检查是否正在运行
 * @retval 1=运行中, 0=停止
 */
uint8_t APP_Temp_IsRunning(void);

/**
 * @brief  设置电流源
 * @param  src: 电流源选择 (0:10μA, 1:17μA)
 * @retval 无
 */
void APP_Temp_SetCurrentSource(uint8_t src);

/**
 * @brief  获取电流源选择
 * @retval 电流源 (0:10μA, 1:17μA)
 */
uint8_t APP_Temp_GetCurrentSource(void);

/**
 * @brief  分度表查表
 * @param  voltage: 电压值 (mV)
 * @retval 温度值 (K)
 */
float APP_Temp_TableLookup(float voltage);

/**
 * @brief  验证分度表有效性
 * @retval 0=有效, -1=无效
 */
int APP_Temp_TableVerify(void);

/**
 * @brief  获取采样计数
 * @retval 采样计数
 */
uint32_t APP_Temp_GetSampleCount(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_TEMP_H */
