/**
 * @file    app_param.h
 * @brief   参数管理应用层头文件
 * @details 提供参数存储、加载、管理功能
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __APP_PARAM_H
#define __APP_PARAM_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 宏定义 --------------------------------------------------------------------*/

/* 参数魔数 */
#define PARAM_MAGIC             0x544D5032  /* "TMP2" */

/* 参数版本 */
#define PARAM_VERSION           0x0100      /* V1.0 */

/* 默认参数值 */
#define DEFAULT_CURRENT_SOURCE  0           /* 默认10μA */
#define DEFAULT_CURRENT_ADJ_10  0.0f        /* 10μA调整值 */
#define DEFAULT_CURRENT_ADJ_17  0.0f        /* 17μA调整值 */
#define DEFAULT_TEMP_4MA        (-200.0f)   /* 4mA对应温度 */
#define DEFAULT_TEMP_20MA       100.0f      /* 20mA对应温度 */

/* 类型定义 ------------------------------------------------------------------*/

/* 用户参数结构体 */
typedef struct {
    uint32_t magic;             /* 魔数 0x544D5032 ("TMP2") */
    uint16_t version;           /* 参数版本 */
    uint16_t reserved;          /* 保留 */
    uint8_t current_source;     /* 电流源选择 (0:10μA, 1:17μA) */
    uint8_t padding[3];         /* 对齐填充 */
    float current_adj_10uA;     /* 10μA调整值 (μA) */
    float current_adj_17uA;     /* 17μA调整值 (μA) */
    float temp_4mA;             /* 4mA对应温度 (℃) */
    float temp_20mA;            /* 20mA对应温度 (℃) */
    uint16_t crc;               /* CRC16校验 */
    uint16_t padding2;          /* 对齐填充 */
} UserParam_t;

/* 函数声明 ------------------------------------------------------------------*/

/**
 * @brief  参数模块初始化
 * @retval 无
 */
void APP_Param_Init(void);

/**
 * @brief  加载参数
 * @retval 0=成功, -1=失败（使用默认值）
 */
int APP_Param_Load(void);

/**
 * @brief  保存参数
 * @retval 0=成功, -1=失败
 */
int APP_Param_Save(void);

/**
 * @brief  恢复默认参数
 * @retval 无
 */
void APP_Param_SetDefault(void);

/**
 * @brief  获取电流源选择
 * @retval 电流源 (0:10μA, 1:17μA)
 */
uint8_t APP_Param_GetCurrentSource(void);

/**
 * @brief  设置电流源选择
 * @param  src: 电流源 (0:10μA, 1:17μA)
 * @retval 无
 */
void APP_Param_SetCurrentSource(uint8_t src);

/**
 * @brief  获取10μA调整值
 * @retval 调整值 (μA)
 */
float APP_Param_GetCurrentAdj10(void);

/**
 * @brief  设置10μA调整值
 * @param  adj: 调整值 (μA)
 * @retval 无
 */
void APP_Param_SetCurrentAdj10(float adj);

/**
 * @brief  获取17μA调整值
 * @retval 调整值 (μA)
 */
float APP_Param_GetCurrentAdj17(void);

/**
 * @brief  设置17μA调整值
 * @param  adj: 调整值 (μA)
 * @retval 无
 */
void APP_Param_SetCurrentAdj17(float adj);

/**
 * @brief  获取4mA对应温度
 * @retval 温度值 (℃)
 */
float APP_Param_Get4mATemp(void);

/**
 * @brief  设置4mA对应温度
 * @param  temp: 温度值 (℃)
 * @retval 无
 */
void APP_Param_Set4mATemp(float temp);

/**
 * @brief  获取20mA对应温度
 * @retval 温度值 (℃)
 */
float APP_Param_Get20mATemp(void);

/**
 * @brief  设置20mA对应温度
 * @param  temp: 温度值 (℃)
 * @retval 无
 */
void APP_Param_Set20mATemp(float temp);

/**
 * @brief  获取参数结构体指针
 * @retval 参数结构体指针
 */
UserParam_t* APP_Param_GetStruct(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_PARAM_H */
