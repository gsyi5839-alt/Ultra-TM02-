/**
 * @file    svc_dac.h
 * @brief   DAC服务层头文件
 * @details 提供DAC输出服务，包括电流源控制和4-20mA输出
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __SVC_DAC_H
#define __SVC_DAC_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 宏定义 --------------------------------------------------------------------*/

/* DAC参数配置 */
#define DAC_VREF            6.5f            /* DAC参考电压 (V) */
#define DAC_FULLSCALE       65536.0f        /* 16位满量程 2^16 */
#define DAC_BITS            16              /* DAC位数 */

/* 电流源标称值 */
#define CURRENT_10UA_NOMINAL    10.0f       /* 10μA标称值 */
#define CURRENT_17UA_NOMINAL    17.0f       /* 17μA标称值 */

/* 4-20mA输出范围 */
#define OUTPUT_CURRENT_MIN      4.0f        /* 最小输出电流 (mA) */
#define OUTPUT_CURRENT_MAX      20.0f       /* 最大输出电流 (mA) */

/* V/I转换系数（根据实际电路确定） */
#define VI_COEFFICIENT          2.5f        /* mA/V */

/* 类型定义 ------------------------------------------------------------------*/

/* 电流源选择 */
typedef enum {
    CURRENT_SRC_10UA = 0,   /* 10μA电流源 */
    CURRENT_SRC_17UA = 1    /* 17μA电流源 */
} CurrentSource_e;

/* DAC通道 */
typedef enum {
    DAC_CHANNEL_1 = 1,      /* DAC1 - 电流源控制 */
    DAC_CHANNEL_2 = 2       /* DAC2 - 4-20mA输出 */
} DACChannel_e;

/* 函数声明 ------------------------------------------------------------------*/

/**
 * @brief  DAC服务初始化
 * @retval 无
 */
void SVC_DAC_Init(void);

/**
 * @brief  设置电流源
 * @param  src: 电流源选择 (CURRENT_SRC_10UA 或 CURRENT_SRC_17UA)
 * @retval 无
 */
void SVC_DAC_SetCurrentSource(CurrentSource_e src);

/**
 * @brief  获取当前电流源选择
 * @retval 当前电流源
 */
CurrentSource_e SVC_DAC_GetCurrentSource(void);

/**
 * @brief  设置电流源调整值
 * @param  src: 电流源选择
 * @param  adj_uA: 调整值 (μA), 正值增大，负值减小
 * @retval 无
 */
void SVC_DAC_SetCurrentAdj(CurrentSource_e src, float adj_uA);

/**
 * @brief  设置4-20mA输出电流
 * @param  current_mA: 输出电流值 (mA), 范围4.0-20.0
 * @retval 无
 */
void SVC_DAC_Set420mA(float current_mA);

/**
 * @brief  获取当前4-20mA输出值
 * @retval 当前输出电流 (mA)
 */
float SVC_DAC_Get420mA(void);

/**
 * @brief  设置DAC电压输出
 * @param  channel: DAC通道 (DAC_CHANNEL_1 或 DAC_CHANNEL_2)
 * @param  voltage: 输出电压 (V)
 * @retval 无
 */
void SVC_DAC_SetVoltage(DACChannel_e channel, float voltage);

/**
 * @brief  设置DAC原始值
 * @param  channel: DAC通道
 * @param  value: 16位DAC值 (0-65535)
 * @retval 无
 */
void SVC_DAC_WriteRaw(DACChannel_e channel, uint16_t value);

/**
 * @brief  触发DAC加载（更新输出）
 * @param  channel: DAC通道
 * @retval 无
 */
void SVC_DAC_Load(DACChannel_e channel);

#ifdef __cplusplus
}
#endif

#endif /* __SVC_DAC_H */
