/**
 * @file    app_output.c
 * @brief   4-20mA输出应用层源文件
 * @details 实现4-20mA电流输出控制功能
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "app_output.h"
#include "app_param.h"
#include "svc_dac.h"
#include "svc_lcd.h"

/* 私有变量 ------------------------------------------------------------------*/

/* 输出配置 */
static OutputConfig_t g_output = {
    .temp_4mA = OUTPUT_DEFAULT_TEMP_4MA,
    .temp_20mA = OUTPUT_DEFAULT_TEMP_20MA,
    .current_mA = OUTPUT_MIN_CURRENT
};

/* 私有函数 ------------------------------------------------------------------*/

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @brief  输出模块初始化
 * @retval 无
 */
void APP_Output_Init(void)
{
    /* 从参数模块加载配置 */
    g_output.temp_4mA = APP_Param_Get4mATemp();
    g_output.temp_20mA = APP_Param_Get20mATemp();
    g_output.current_mA = OUTPUT_MIN_CURRENT;
    
    /* 设置初始输出为4mA */
    SVC_DAC_Set420mA(OUTPUT_MIN_CURRENT);
}

/**
 * @brief  根据温度更新输出电流
 * @param  temperature: 温度值 (℃)
 * @retval 无
 */
void APP_Output_UpdateCurrent(float temperature)
{
    float current;
    
    /* 计算输出电流 */
    current = APP_Output_CalcCurrent(temperature);
    
    /* 保存当前值 */
    g_output.current_mA = current;
    
    /* 设置DAC输出 */
    SVC_DAC_Set420mA(current);
    
    /* 更新LCD显示 */
    SVC_LCD_SetCurrent(current);
}

/**
 * @brief  直接设置输出电流
 * @param  current_mA: 电流值 (mA)
 * @retval 无
 */
void APP_Output_SetCurrent(float current_mA)
{
    /* 限幅 */
    if (current_mA < OUTPUT_MIN_CURRENT)
    {
        current_mA = OUTPUT_MIN_CURRENT;
    }
    if (current_mA > OUTPUT_MAX_CURRENT)
    {
        current_mA = OUTPUT_MAX_CURRENT;
    }
    
    /* 保存并输出 */
    g_output.current_mA = current_mA;
    SVC_DAC_Set420mA(current_mA);
    SVC_LCD_SetCurrent(current_mA);
}

/**
 * @brief  获取当前输出电流
 * @retval 电流值 (mA)
 */
float APP_Output_GetCurrent(void)
{
    return g_output.current_mA;
}

/**
 * @brief  设置4mA对应温度点
 * @param  temp: 温度值 (℃)
 * @retval 无
 */
void APP_Output_Set4mATemp(float temp)
{
    g_output.temp_4mA = temp;
}

/**
 * @brief  获取4mA对应温度点
 * @retval 温度值 (℃)
 */
float APP_Output_Get4mATemp(void)
{
    return g_output.temp_4mA;
}

/**
 * @brief  设置20mA对应温度点
 * @param  temp: 温度值 (℃)
 * @retval 无
 */
void APP_Output_Set20mATemp(float temp)
{
    g_output.temp_20mA = temp;
}

/**
 * @brief  获取20mA对应温度点
 * @retval 温度值 (℃)
 */
float APP_Output_Get20mATemp(void)
{
    return g_output.temp_20mA;
}

/**
 * @brief  根据温度计算输出电流
 * @param  temperature: 温度值 (℃)
 * @retval 计算得到的电流值 (mA)
 * @note   使用线性插值公式：
 *         I = 4 + (T - T_4mA) / (T_20mA - T_4mA) * 16
 */
float APP_Output_CalcCurrent(float temperature)
{
    float current;
    float temp_range;
    
    /* 计算温度范围 */
    temp_range = g_output.temp_20mA - g_output.temp_4mA;
    
    /* 避免除零 */
    if (temp_range == 0.0f)
    {
        return 12.0f;  /* 返回中间值 */
    }
    
    /* 线性插值计算 */
    current = OUTPUT_MIN_CURRENT + 
              (temperature - g_output.temp_4mA) / temp_range * 
              (OUTPUT_MAX_CURRENT - OUTPUT_MIN_CURRENT);
    
    /* 限幅 */
    if (current < OUTPUT_MIN_CURRENT)
    {
        current = OUTPUT_MIN_CURRENT;
    }
    if (current > OUTPUT_MAX_CURRENT)
    {
        current = OUTPUT_MAX_CURRENT;
    }
    
    return current;
}

/**
 * @brief  获取输出配置
 * @param  config: 配置结构体指针
 * @retval 无
 */
void APP_Output_GetConfig(OutputConfig_t *config)
{
    if (config != NULL)
    {
        config->temp_4mA = g_output.temp_4mA;
        config->temp_20mA = g_output.temp_20mA;
        config->current_mA = g_output.current_mA;
    }
}

/**
 * @brief  设置输出配置
 * @param  config: 配置结构体指针
 * @retval 无
 */
void APP_Output_SetConfig(OutputConfig_t *config)
{
    if (config != NULL)
    {
        g_output.temp_4mA = config->temp_4mA;
        g_output.temp_20mA = config->temp_20mA;
    }
}
