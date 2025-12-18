/**
 * @file    svc_adc.c
 * @brief   ADC服务层源文件
 * @details 实现24位高精度ADC采集服务，用于温度信号采集
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "svc_adc.h"
#include "bsp_spi.h"
#include "bsp_gpio.h"

/* 私有变量 ------------------------------------------------------------------*/

/* ADC配置 */
static ADCConfig_t adc_config = {
    .gain = ADC_GAIN_1,
    .sample_rate = 0,
    .vref = ADC_VREF
};

/* 当前增益系数 */
static float gain_factor = 1.0f;

/* 私有函数 ------------------------------------------------------------------*/

/**
 * @brief  根据增益设置计算增益系数
 * @param  gain: 增益设置值
 * @retval 增益系数
 */
static float CalcGainFactor(uint8_t gain)
{
    switch (gain)
    {
        case ADC_GAIN_1:   return 1.0f;
        case ADC_GAIN_2:   return 2.0f;
        case ADC_GAIN_4:   return 4.0f;
        case ADC_GAIN_8:   return 8.0f;
        case ADC_GAIN_16:  return 16.0f;
        case ADC_GAIN_32:  return 32.0f;
        case ADC_GAIN_64:  return 64.0f;
        case ADC_GAIN_128: return 128.0f;
        default:           return 1.0f;
    }
}

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @brief  ADC服务初始化
 * @note   配置ADC芯片，设置默认参数
 * @retval 无
 */
void SVC_ADC_Init(void)
{
    uint8_t config_data;
    
    /* 确保片选为高电平 */
    BSP_ADC_CS(1);
    HAL_Delay(1);
    
    /* 复位ADC（根据实际ADC芯片协议） */
    BSP_ADC_CS(0);
    BSP_SPI_TransmitReceive(0xFF);  /* 发送复位命令 */
    BSP_ADC_CS(1);
    HAL_Delay(10);
    
    /* 配置ADC参数 */
    /* 设置增益=1，采样率=1SPS */
    config_data = (ADC_GAIN_1 << 4) | 0x00;  /* 根据实际ADC芯片修改 */
    SVC_ADC_WriteReg(ADC_REG_CONFIG, config_data);
    
    /* 更新配置 */
    adc_config.gain = ADC_GAIN_1;
    gain_factor = 1.0f;
}

/**
 * @brief  启动ADC转换
 * @retval 无
 */
void SVC_ADC_StartConversion(void)
{
    /* 发送启动转换命令（根据实际ADC芯片协议） */
    BSP_ADC_CS(0);
    BSP_SPI_TransmitReceive(0x08);  /* 启动命令示例 */
    BSP_ADC_CS(1);
}

/**
 * @brief  检查ADC数据是否就绪
 * @retval 1=就绪, 0=未就绪
 */
uint8_t SVC_ADC_IsReady(void)
{
    /* 读取DRDY引脚状态 */
    return BSP_ADC_IsDataReady();
}

/**
 * @brief  读取ADC原始值
 * @retval 24位ADC原始值
 */
uint32_t SVC_ADC_ReadRaw(void)
{
    uint8_t rx_buf[3];
    uint32_t raw_value;
    
    /* 读取数据寄存器 */
    BSP_ADC_CS(0);
    
    /* 发送读取数据命令 */
    BSP_SPI_TransmitReceive(ADC_REG_DATA | 0x40);  /* 读命令 */
    
    /* 读取3个字节（24位） */
    rx_buf[0] = BSP_SPI_TransmitReceive(0x00);  /* MSB */
    rx_buf[1] = BSP_SPI_TransmitReceive(0x00);
    rx_buf[2] = BSP_SPI_TransmitReceive(0x00);  /* LSB */
    
    BSP_ADC_CS(1);
    
    /* 组合24位数据 (MSB first) */
    raw_value = ((uint32_t)rx_buf[0] << 16) | 
                ((uint32_t)rx_buf[1] << 8) | 
                rx_buf[2];
    
    return raw_value;
}

/**
 * @brief  读取ADC电压值
 * @retval 电压值 (mV)
 */
float SVC_ADC_ReadVoltage(void)
{
    uint32_t raw;
    float voltage;
    int32_t signed_raw;
    
    /* 读取原始值 */
    raw = SVC_ADC_ReadRaw();
    
    /* 转换为有符号值（假设差分输入，中点为Vref/2） */
    /* 24位ADC，中点值为0x800000 */
    signed_raw = (int32_t)(raw - 0x800000);
    
    /* 计算电压 (mV) */
    /* Voltage = (ADC_Value / FullScale) * Vref * 1000 / Gain */
    voltage = ((float)signed_raw / (ADC_FULLSCALE / 2.0f)) * 
              (adc_config.vref / 2.0f) * 1000.0f / gain_factor;
    
    return voltage;
}

/**
 * @brief  设置ADC增益
 * @param  gain: 增益值 (ADC_GAIN_x)
 * @retval 无
 */
void SVC_ADC_SetGain(uint8_t gain)
{
    uint8_t config_data;
    
    /* 验证增益值 */
    if (gain > ADC_GAIN_128)
    {
        gain = ADC_GAIN_1;
    }
    
    /* 读取当前配置 */
    config_data = SVC_ADC_ReadReg(ADC_REG_CONFIG);
    
    /* 更新增益位 */
    config_data = (config_data & 0x0F) | (gain << 4);
    
    /* 写入配置 */
    SVC_ADC_WriteReg(ADC_REG_CONFIG, config_data);
    
    /* 更新本地配置 */
    adc_config.gain = gain;
    gain_factor = CalcGainFactor(gain);
}

/**
 * @brief  获取当前ADC增益
 * @retval 当前增益系数
 */
float SVC_ADC_GetGain(void)
{
    return gain_factor;
}

/**
 * @brief  设置参考电压值
 * @param  vref: 参考电压 (V)
 * @note   可根据实际测量值校准
 * @retval 无
 */
void SVC_ADC_SetVref(float vref)
{
    if (vref > 0 && vref < 10.0f)
    {
        adc_config.vref = vref;
    }
}

/**
 * @brief  写入ADC寄存器
 * @param  reg: 寄存器地址
 * @param  data: 数据
 * @retval 无
 */
void SVC_ADC_WriteReg(uint8_t reg, uint8_t data)
{
    BSP_ADC_CS(0);
    
    /* 发送写命令 + 寄存器地址 */
    BSP_SPI_TransmitReceive(reg & 0x3F);  /* 写命令 */
    
    /* 发送数据 */
    BSP_SPI_TransmitReceive(data);
    
    BSP_ADC_CS(1);
}

/**
 * @brief  读取ADC寄存器
 * @param  reg: 寄存器地址
 * @retval 寄存器值
 */
uint8_t SVC_ADC_ReadReg(uint8_t reg)
{
    uint8_t data;
    
    BSP_ADC_CS(0);
    
    /* 发送读命令 + 寄存器地址 */
    BSP_SPI_TransmitReceive(reg | 0x40);  /* 读命令 */
    
    /* 读取数据 */
    data = BSP_SPI_TransmitReceive(0x00);
    
    BSP_ADC_CS(1);
    
    return data;
}
