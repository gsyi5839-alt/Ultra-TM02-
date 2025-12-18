/**
 * @file    svc_dac.c
 * @brief   DAC服务层源文件
 * @details 实现DAC输出服务，包括电流源控制和4-20mA输出
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "svc_dac.h"
#include "bsp_spi.h"
#include "bsp_gpio.h"

/* 私有变量 ------------------------------------------------------------------*/

/* 当前电流源选择 */
static CurrentSource_e current_source = CURRENT_SRC_10UA;

/* 电流源调整值 (μA) */
static float current_adj_10uA = 0.0f;
static float current_adj_17uA = 0.0f;

/* 当前4-20mA输出值 */
static float output_current_mA = 4.0f;

/* 私有函数 ------------------------------------------------------------------*/

/**
 * @brief  电压值转换为DAC原始值
 * @param  voltage: 电压值 (V)
 * @retval 16位DAC值
 */
static uint16_t VoltageToDAC(float voltage)
{
    float dac_value;
    
    /* 限制电压范围 */
    if (voltage < 0) voltage = 0;
    if (voltage > DAC_VREF) voltage = DAC_VREF;
    
    /* 计算DAC值 */
    dac_value = (voltage / DAC_VREF) * DAC_FULLSCALE;
    
    if (dac_value > 65535.0f) dac_value = 65535.0f;
    
    return (uint16_t)dac_value;
}

/**
 * @brief  发送数据到DAC芯片
 * @param  channel: DAC通道
 * @param  value: 16位DAC值
 * @retval 无
 */
static void DAC_Write(DACChannel_e channel, uint16_t value)
{
    uint8_t buf[3];
    
    /* 准备数据（根据实际DAC芯片协议调整） */
    buf[0] = 0x00;                  /* 写命令 */
    buf[1] = (value >> 8) & 0xFF;   /* 高8位 */
    buf[2] = value & 0xFF;          /* 低8位 */
    
    if (channel == DAC_CHANNEL_1)
    {
        /* DAC1 操作 */
        BSP_DAC1_CS(0);
        BSP_SPI_Transmit(buf, 3);
        BSP_DAC1_CS(1);
    }
    else
    {
        /* DAC2 操作 */
        BSP_DAC2_CS(0);
        BSP_SPI_Transmit(buf, 3);
        BSP_DAC2_CS(1);
    }
}

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @brief  DAC服务初始化
 * @retval 无
 */
void SVC_DAC_Init(void)
{
    /* 确保所有片选和LOAD信号为高电平 */
    BSP_DAC1_CS(1);
    BSP_DAC2_CS(1);
    BSP_DAC1_LOAD(1);
    BSP_DAC2_LOAD(1);
    
    HAL_Delay(1);
    
    /* 设置DAC1输出为0（电流源关闭） */
    SVC_DAC_WriteRaw(DAC_CHANNEL_1, 0);
    SVC_DAC_Load(DAC_CHANNEL_1);
    
    /* 设置DAC2输出4mA对应值 */
    SVC_DAC_Set420mA(OUTPUT_CURRENT_MIN);
    
    /* 默认选择10μA电流源 */
    current_source = CURRENT_SRC_10UA;
}

/**
 * @brief  设置电流源
 * @param  src: 电流源选择 (CURRENT_SRC_10UA 或 CURRENT_SRC_17UA)
 * @retval 无
 */
void SVC_DAC_SetCurrentSource(CurrentSource_e src)
{
    float target_current;
    float voltage;
    uint16_t dac_value;
    
    /* 保存当前选择 */
    current_source = src;
    
    /* 计算目标电流（含调整值） */
    if (src == CURRENT_SRC_10UA)
    {
        target_current = CURRENT_10UA_NOMINAL + current_adj_10uA;
    }
    else
    {
        target_current = CURRENT_17UA_NOMINAL + current_adj_17uA;
    }
    
    /* 确保电流为正值 */
    if (target_current < 0) target_current = 0;
    
    /* 根据V/I电路计算所需电压 */
    /* 假设电流源电路：I_out = V_DAC * K，其中K为转换系数 */
    /* 实际系数需要根据硬件电路确定 */
    voltage = target_current / 1000000.0f * 1000.0f;  /* μA转换为mA再计算 */
    
    /* 转换为DAC值 */
    dac_value = VoltageToDAC(voltage);
    
    /* 写入DAC1 */
    DAC_Write(DAC_CHANNEL_1, dac_value);
    
    /* 触发加载 */
    SVC_DAC_Load(DAC_CHANNEL_1);
}

/**
 * @brief  获取当前电流源选择
 * @retval 当前电流源
 */
CurrentSource_e SVC_DAC_GetCurrentSource(void)
{
    return current_source;
}

/**
 * @brief  设置电流源调整值
 * @param  src: 电流源选择
 * @param  adj_uA: 调整值 (μA), 正值增大，负值减小
 * @retval 无
 */
void SVC_DAC_SetCurrentAdj(CurrentSource_e src, float adj_uA)
{
    /* 限制调整范围 ±1μA */
    if (adj_uA > 1.0f) adj_uA = 1.0f;
    if (adj_uA < -1.0f) adj_uA = -1.0f;
    
    if (src == CURRENT_SRC_10UA)
    {
        current_adj_10uA = adj_uA;
    }
    else
    {
        current_adj_17uA = adj_uA;
    }
    
    /* 如果当前正在使用该电流源，则立即更新 */
    if (current_source == src)
    {
        SVC_DAC_SetCurrentSource(src);
    }
}

/**
 * @brief  设置4-20mA输出电流
 * @param  current_mA: 输出电流值 (mA), 范围4.0-20.0
 * @retval 无
 */
void SVC_DAC_Set420mA(float current_mA)
{
    float voltage;
    uint16_t dac_value;
    
    /* 限制电流范围 */
    if (current_mA < OUTPUT_CURRENT_MIN) current_mA = OUTPUT_CURRENT_MIN;
    if (current_mA > OUTPUT_CURRENT_MAX) current_mA = OUTPUT_CURRENT_MAX;
    
    /* 保存当前值 */
    output_current_mA = current_mA;
    
    /* 计算DAC电压 */
    /* 根据V/I转换电路：I_out = V_DAC * VI_COEFFICIENT */
    voltage = current_mA / VI_COEFFICIENT;
    
    /* 转换为DAC值 */
    dac_value = VoltageToDAC(voltage);
    
    /* 写入DAC2 */
    DAC_Write(DAC_CHANNEL_2, dac_value);
    
    /* 触发加载 */
    SVC_DAC_Load(DAC_CHANNEL_2);
}

/**
 * @brief  获取当前4-20mA输出值
 * @retval 当前输出电流 (mA)
 */
float SVC_DAC_Get420mA(void)
{
    return output_current_mA;
}

/**
 * @brief  设置DAC电压输出
 * @param  channel: DAC通道 (DAC_CHANNEL_1 或 DAC_CHANNEL_2)
 * @param  voltage: 输出电压 (V)
 * @retval 无
 */
void SVC_DAC_SetVoltage(DACChannel_e channel, float voltage)
{
    uint16_t dac_value;
    
    /* 转换为DAC值 */
    dac_value = VoltageToDAC(voltage);
    
    /* 写入DAC */
    DAC_Write(channel, dac_value);
    
    /* 触发加载 */
    SVC_DAC_Load(channel);
}

/**
 * @brief  设置DAC原始值
 * @param  channel: DAC通道
 * @param  value: 16位DAC值 (0-65535)
 * @retval 无
 */
void SVC_DAC_WriteRaw(DACChannel_e channel, uint16_t value)
{
    DAC_Write(channel, value);
}

/**
 * @brief  触发DAC加载（更新输出）
 * @param  channel: DAC通道
 * @retval 无
 */
void SVC_DAC_Load(DACChannel_e channel)
{
    if (channel == DAC_CHANNEL_1)
    {
        /* DAC1 LOAD信号：低脉冲触发 */
        BSP_DAC1_LOAD(0);
        HAL_Delay(1);
        BSP_DAC1_LOAD(1);
    }
    else
    {
        /* DAC2 LOAD信号：低脉冲触发 */
        BSP_DAC2_LOAD(0);
        HAL_Delay(1);
        BSP_DAC2_LOAD(1);
    }
}
