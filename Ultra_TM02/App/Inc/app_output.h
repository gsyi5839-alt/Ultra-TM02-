/**
 * @file    app_output.h
 * @brief   4-20mA输出应用层头文件
 * @details 提供4-20mA电流输出控制功能
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __APP_OUTPUT_H
#define __APP_OUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 宏定义 --------------------------------------------------------------------*/

/* 输出电流范围 */
#define OUTPUT_MIN_CURRENT      4.0f        /* 最小输出电流 (mA) */
#define OUTPUT_MAX_CURRENT      20.0f       /* 最大输出电流 (mA) */

/* 默认温度范围 */
#define OUTPUT_DEFAULT_TEMP_4MA     -200.0f /* 4mA默认对应温度 (℃) */
#define OUTPUT_DEFAULT_TEMP_20MA    100.0f  /* 20mA默认对应温度 (℃) */

/* 类型定义 ------------------------------------------------------------------*/

/* 输出配置结构体 */
typedef struct {
    float temp_4mA;             /* 4mA对应温度 (℃) */
    float temp_20mA;            /* 20mA对应温度 (℃) */
    float current_mA;           /* 当前输出电流 (mA) */
} OutputConfig_t;

/* 函数声明 ------------------------------------------------------------------*/

/**
 * @brief  输出模块初始化
 * @retval 无
 */
void APP_Output_Init(void);

/**
 * @brief  根据温度更新输出电流
 * @param  temperature: 温度值 (℃)
 * @retval 无
 */
void APP_Output_UpdateCurrent(float temperature);

/**
 * @brief  直接设置输出电流
 * @param  current_mA: 电流值 (mA)
 * @retval 无
 */
void APP_Output_SetCurrent(float current_mA);

/**
 * @brief  获取当前输出电流
 * @retval 电流值 (mA)
 */
float APP_Output_GetCurrent(void);

/**
 * @brief  设置4mA对应温度点
 * @param  temp: 温度值 (℃)
 * @retval 无
 */
void APP_Output_Set4mATemp(float temp);

/**
 * @brief  获取4mA对应温度点
 * @retval 温度值 (℃)
 */
float APP_Output_Get4mATemp(void);

/**
 * @brief  设置20mA对应温度点
 * @param  temp: 温度值 (℃)
 * @retval 无
 */
void APP_Output_Set20mATemp(float temp);

/**
 * @brief  获取20mA对应温度点
 * @retval 温度值 (℃)
 */
float APP_Output_Get20mATemp(void);

/**
 * @brief  根据温度计算输出电流
 * @param  temperature: 温度值 (℃)
 * @retval 计算得到的电流值 (mA)
 */
float APP_Output_CalcCurrent(float temperature);

/**
 * @brief  获取输出配置
 * @param  config: 配置结构体指针
 * @retval 无
 */
void APP_Output_GetConfig(OutputConfig_t *config);

/**
 * @brief  设置输出配置
 * @param  config: 配置结构体指针
 * @retval 无
 */
void APP_Output_SetConfig(OutputConfig_t *config);

#ifdef __cplusplus
}
#endif

#endif /* __APP_OUTPUT_H */
