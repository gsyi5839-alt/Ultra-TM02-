/**
 * @file    svc_adc.h
 * @brief   ADC服务层头文件
 * @details 提供24位高精度ADC采集服务，用于温度信号采集
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __SVC_ADC_H
#define __SVC_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 宏定义 --------------------------------------------------------------------*/

/**
 * ADC参数配置
 * 注意：ADC_VREF需要根据实际测量TP4 (6.5VA)进行调整
 */
#define ADC_VREF            6.5f            /* 参考电压 (V) */
#define ADC_FULLSCALE       16777216.0f     /* 24位满量程 2^24 */
#define ADC_BITS            24              /* ADC位数 */

/* ADC寄存器定义（根据实际ADC芯片修改） */
#define ADC_REG_STATUS      0x00
#define ADC_REG_CONFIG      0x01
#define ADC_REG_DATA        0x02
#define ADC_REG_GAIN        0x03

/* ADC增益定义 */
#define ADC_GAIN_1          0x00
#define ADC_GAIN_2          0x01
#define ADC_GAIN_4          0x02
#define ADC_GAIN_8          0x03
#define ADC_GAIN_16         0x04
#define ADC_GAIN_32         0x05
#define ADC_GAIN_64         0x06
#define ADC_GAIN_128        0x07

/* 类型定义 ------------------------------------------------------------------*/

/* ADC状态 */
typedef enum {
    ADC_STATE_IDLE = 0,     /* 空闲 */
    ADC_STATE_CONVERTING,   /* 转换中 */
    ADC_STATE_READY,        /* 数据就绪 */
    ADC_STATE_ERROR         /* 错误 */
} ADCState_t;

/* ADC配置结构体 */
typedef struct {
    uint8_t gain;           /* 增益设置 (ADC_GAIN_x) */
    uint8_t sample_rate;    /* 采样率设置 */
    float vref;             /* 参考电压 */
} ADCConfig_t;

/* 函数声明 ------------------------------------------------------------------*/

/**
 * @brief  ADC服务初始化
 * @retval 无
 */
void SVC_ADC_Init(void);

/**
 * @brief  启动ADC转换
 * @retval 无
 */
void SVC_ADC_StartConversion(void);

/**
 * @brief  检查ADC数据是否就绪
 * @retval 1=就绪, 0=未就绪
 */
uint8_t SVC_ADC_IsReady(void);

/**
 * @brief  读取ADC原始值
 * @retval 24位ADC原始值
 */
uint32_t SVC_ADC_ReadRaw(void);

/**
 * @brief  读取ADC电压值
 * @retval 电压值 (mV)
 */
float SVC_ADC_ReadVoltage(void);

/**
 * @brief  设置ADC增益
 * @param  gain: 增益值 (ADC_GAIN_x)
 * @retval 无
 */
void SVC_ADC_SetGain(uint8_t gain);

/**
 * @brief  获取当前ADC增益
 * @retval 当前增益值
 */
float SVC_ADC_GetGain(void);

/**
 * @brief  设置参考电压值
 * @param  vref: 参考电压 (V)
 * @note   可根据实际测量值校准
 * @retval 无
 */
void SVC_ADC_SetVref(float vref);

/**
 * @brief  写入ADC寄存器
 * @param  reg: 寄存器地址
 * @param  data: 数据
 * @retval 无
 */
void SVC_ADC_WriteReg(uint8_t reg, uint8_t data);

/**
 * @brief  读取ADC寄存器
 * @param  reg: 寄存器地址
 * @retval 寄存器值
 */
uint8_t SVC_ADC_ReadReg(uint8_t reg);

#ifdef __cplusplus
}
#endif

#endif /* __SVC_ADC_H */
