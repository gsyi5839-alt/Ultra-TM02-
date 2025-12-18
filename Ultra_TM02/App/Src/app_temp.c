/**
 * @file    app_temp.c
 * @brief   温度测量应用层源文件
 * @details 实现温度测量、滤波、查表等功能
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "app_temp.h"
#include "app_output.h"
#include "svc_adc.h"
#include "svc_dac.h"
#include "svc_lcd.h"
#include <string.h>

/* 私有宏定义 ----------------------------------------------------------------*/

/* 探头检测阈值 */
#define PROBE_OPEN_VOLTAGE      3000.0f     /* 断开阈值 (mV) */
#define PROBE_SHORT_VOLTAGE     10.0f       /* 短路阈值 (mV) */
#define PROBE_MAX_VOLTAGE       2500.0f     /* 最大有效电压 (mV) */
#define PROBE_MIN_VOLTAGE       100.0f      /* 最小有效电压 (mV) */

/* 私有变量 ------------------------------------------------------------------*/

/* 温度测量数据 */
static TempMeasure_t g_temp = {
    .state = TEMP_STATE_IDLE,
    .probe_status = PROBE_STATUS_OK,
    .current_src = 0,
    .running = 0,
    .raw_voltage = 0.0f,
    .filtered_voltage = 0.0f,
    .temperature_K = 0.0f,
    .temperature_C = 0.0f,
    .sample_count = 0
};

/* 采样缓冲区 */
static float sample_buffer[TEMP_SAMPLE_COUNT];
static uint8_t sample_index = 0;

/* 滑动平均滤波器 */
static float filter_buffer[TEMP_FILTER_SIZE];
static uint8_t filter_index = 0;
static uint8_t filter_count = 0;
static float filter_sum = 0.0f;

/* 分度表指针（指向Flash） */
static TempTableHeader_t *p_table_header = (TempTableHeader_t *)TEMP_TABLE_FLASH_ADDR;
static TempTablePoint_t *p_table_points = (TempTablePoint_t *)(TEMP_TABLE_FLASH_ADDR + sizeof(TempTableHeader_t));

/* 私有函数声明 --------------------------------------------------------------*/
static float MedianFilter(float *data, uint8_t len);
static float MovingAvgFilter(float value);
static void CheckProbeStatus(float voltage);
static float Kelvin_to_Celsius(float kelvin);

/* 私有函数 ------------------------------------------------------------------*/

/**
 * @brief  中值滤波
 * @param  data: 数据数组
 * @param  len: 数据长度
 * @retval 中值
 */
static float MedianFilter(float *data, uint8_t len)
{
    float temp[TEMP_SAMPLE_COUNT];
    float t;
    uint8_t i, j;
    
    /* 复制数据 */
    for (i = 0; i < len && i < TEMP_SAMPLE_COUNT; i++)
    {
        temp[i] = data[i];
    }
    
    /* 冒泡排序 */
    for (i = 0; i < len - 1; i++)
    {
        for (j = 0; j < len - 1 - i; j++)
        {
            if (temp[j] > temp[j + 1])
            {
                t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }
    
    /* 返回中值 */
    return temp[len / 2];
}

/**
 * @brief  滑动平均滤波
 * @param  value: 新数据
 * @retval 滤波后的值
 */
static float MovingAvgFilter(float value)
{
    /* 减去旧值 */
    filter_sum -= filter_buffer[filter_index];
    
    /* 添加新值 */
    filter_buffer[filter_index] = value;
    filter_sum += value;
    
    /* 更新索引 */
    filter_index = (filter_index + 1) % TEMP_FILTER_SIZE;
    
    /* 更新计数 */
    if (filter_count < TEMP_FILTER_SIZE)
    {
        filter_count++;
    }
    
    /* 返回平均值 */
    return filter_sum / filter_count;
}

/**
 * @brief  检查探头状态
 * @param  voltage: 电压值 (mV)
 * @retval 无
 */
static void CheckProbeStatus(float voltage)
{
    if (voltage > PROBE_OPEN_VOLTAGE)
    {
        g_temp.probe_status = PROBE_STATUS_OPEN;
    }
    else if (voltage < PROBE_SHORT_VOLTAGE)
    {
        g_temp.probe_status = PROBE_STATUS_SHORT;
    }
    else if (voltage > PROBE_MAX_VOLTAGE || voltage < PROBE_MIN_VOLTAGE)
    {
        g_temp.probe_status = PROBE_STATUS_RANGE_ERR;
    }
    else
    {
        g_temp.probe_status = PROBE_STATUS_OK;
    }
}

/**
 * @brief  开尔文转摄氏度
 * @param  kelvin: 温度 (K)
 * @retval 温度 (℃)
 */
static float Kelvin_to_Celsius(float kelvin)
{
    return kelvin - 273.15f;
}

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @brief  温度测量初始化
 * @retval 无
 */
void APP_Temp_Init(void)
{
    /* 初始化状态 */
    g_temp.state = TEMP_STATE_IDLE;
    g_temp.probe_status = PROBE_STATUS_OK;
    g_temp.current_src = 0;
    g_temp.running = 0;
    g_temp.raw_voltage = 0.0f;
    g_temp.filtered_voltage = 0.0f;
    g_temp.temperature_K = 0.0f;
    g_temp.temperature_C = 0.0f;
    g_temp.sample_count = 0;
    
    /* 清空滤波器 */
    memset(filter_buffer, 0, sizeof(filter_buffer));
    filter_index = 0;
    filter_count = 0;
    filter_sum = 0.0f;
    
    sample_index = 0;
    
    /* 初始化ADC */
    SVC_ADC_Init();
    
    /* 初始化DAC并设置默认电流源 */
    SVC_DAC_Init();
    SVC_DAC_SetCurrentSource(CURRENT_SRC_10UA);
    
    /* 验证分度表 */
    if (APP_Temp_TableVerify() != 0)
    {
        /* 分度表无效，设置状态 */
        g_temp.state = TEMP_STATE_ERROR;
    }
}

/**
 * @brief  开始温度测量
 * @retval 无
 */
void APP_Temp_Start(void)
{
    g_temp.running = 1;
    g_temp.state = TEMP_STATE_SAMPLING;
    sample_index = 0;
    
    /* 启动ADC转换 */
    SVC_ADC_StartConversion();
    
    /* 更新LCD状态 */
    SVC_LCD_SetStatus("Measuring...");
}

/**
 * @brief  停止温度测量
 * @retval 无
 */
void APP_Temp_Stop(void)
{
    g_temp.running = 0;
    g_temp.state = TEMP_STATE_IDLE;
    
    /* 更新LCD状态 */
    SVC_LCD_SetStatus("Stopped");
}

/**
 * @brief  温度测量处理（主循环中调用）
 * @retval 无
 */
void APP_Temp_Process(void)
{
    float median_value;
    
    /* 检查是否运行中 */
    if (!g_temp.running)
    {
        return;
    }
    
    switch (g_temp.state)
    {
        case TEMP_STATE_SAMPLING:
            /* 检查ADC数据是否就绪 */
            if (SVC_ADC_IsReady())
            {
                /* 读取ADC电压 */
                g_temp.raw_voltage = SVC_ADC_ReadVoltage();
                
                /* 存入采样缓冲区 */
                sample_buffer[sample_index++] = g_temp.raw_voltage;
                
                /* 检查是否采集够了 */
                if (sample_index >= TEMP_SAMPLE_COUNT)
                {
                    sample_index = 0;
                    g_temp.state = TEMP_STATE_FILTERING;
                }
                else
                {
                    /* 启动下一次ADC转换 */
                    SVC_ADC_StartConversion();
                }
            }
            break;
            
        case TEMP_STATE_FILTERING:
            /* 中值滤波 */
            median_value = MedianFilter(sample_buffer, TEMP_SAMPLE_COUNT);
            
            /* 滑动平均滤波 */
            g_temp.filtered_voltage = MovingAvgFilter(median_value);
            
            /* 检查探头状态 */
            CheckProbeStatus(g_temp.filtered_voltage);
            
            g_temp.state = TEMP_STATE_CALCULATING;
            break;
            
        case TEMP_STATE_CALCULATING:
            /* 只有探头正常时才计算温度 */
            if (g_temp.probe_status == PROBE_STATUS_OK)
            {
                /* 查分度表获取温度 */
                g_temp.temperature_K = APP_Temp_TableLookup(g_temp.filtered_voltage);
                
                /* 单位转换 */
                g_temp.temperature_C = Kelvin_to_Celsius(g_temp.temperature_K);
                
                /* 更新LCD显示 */
                SVC_LCD_SetTemperature(g_temp.temperature_C);
                SVC_LCD_SetVoltage(g_temp.filtered_voltage);
                SVC_LCD_SetStatus("OK");
            }
            else
            {
                /* 探头异常，显示错误 */
                switch (g_temp.probe_status)
                {
                    case PROBE_STATUS_OPEN:
                        SVC_LCD_SetStatus("Probe Open!");
                        break;
                    case PROBE_STATUS_SHORT:
                        SVC_LCD_SetStatus("Probe Short!");
                        break;
                    case PROBE_STATUS_RANGE_ERR:
                        SVC_LCD_SetStatus("Out of Range!");
                        break;
                    default:
                        SVC_LCD_SetStatus("Error!");
                        break;
                }
            }
            
            g_temp.state = TEMP_STATE_OUTPUTTING;
            break;
            
        case TEMP_STATE_OUTPUTTING:
            /* 更新4-20mA输出 */
            if (g_temp.probe_status == PROBE_STATUS_OK)
            {
                APP_Output_UpdateCurrent(g_temp.temperature_C);
            }
            
            /* 增加采样计数 */
            g_temp.sample_count++;
            
            /* 启动下一轮采样 */
            g_temp.state = TEMP_STATE_SAMPLING;
            SVC_ADC_StartConversion();
            break;
            
        case TEMP_STATE_ERROR:
            /* 错误状态，等待处理 */
            SVC_LCD_SetStatus("System Error!");
            break;
            
        default:
            g_temp.state = TEMP_STATE_IDLE;
            break;
    }
}

/**
 * @brief  获取当前温度值
 * @retval 温度值 (℃)
 */
float APP_Temp_GetValue(void)
{
    return g_temp.temperature_C;
}

/**
 * @brief  获取当前温度值（开尔文）
 * @retval 温度值 (K)
 */
float APP_Temp_GetValueK(void)
{
    return g_temp.temperature_K;
}

/**
 * @brief  获取当前电压值
 * @retval 电压值 (mV)
 */
float APP_Temp_GetVoltage(void)
{
    return g_temp.filtered_voltage;
}

/**
 * @brief  获取探头状态
 * @retval 探头状态
 */
ProbeStatus_t APP_Temp_GetProbeStatus(void)
{
    return g_temp.probe_status;
}

/**
 * @brief  获取测量状态
 * @retval 测量状态
 */
TempState_t APP_Temp_GetState(void)
{
    return g_temp.state;
}

/**
 * @brief  检查是否正在运行
 * @retval 1=运行中, 0=停止
 */
uint8_t APP_Temp_IsRunning(void)
{
    return g_temp.running;
}

/**
 * @brief  设置电流源
 * @param  src: 电流源选择 (0:10μA, 1:17μA)
 * @retval 无
 */
void APP_Temp_SetCurrentSource(uint8_t src)
{
    g_temp.current_src = src;
    SVC_DAC_SetCurrentSource(src ? CURRENT_SRC_17UA : CURRENT_SRC_10UA);
    SVC_LCD_SetCurrentSource(src);
}

/**
 * @brief  获取电流源选择
 * @retval 电流源 (0:10μA, 1:17μA)
 */
uint8_t APP_Temp_GetCurrentSource(void)
{
    return g_temp.current_src;
}

/**
 * @brief  分度表查表（二分查找+线性插值）
 * @param  voltage: 电压值 (mV)
 * @retval 温度值 (K)
 */
float APP_Temp_TableLookup(float voltage)
{
    int low = 0;
    int high = p_table_header->point_count - 1;
    int mid;
    float v0, v1, t0, t1;
    
    /* 检查分度表有效性 */
    if (APP_Temp_TableVerify() != 0)
    {
        return 0.0f;
    }
    
    /* 边界检查 */
    if (voltage >= p_table_points[0].voltage)
    {
        return p_table_points[0].temperature;
    }
    if (voltage <= p_table_points[high].voltage)
    {
        return p_table_points[high].temperature;
    }
    
    /* 二分查找 */
    while (high - low > 1)
    {
        mid = (low + high) / 2;
        
        if (voltage > p_table_points[mid].voltage)
        {
            high = mid;
        }
        else
        {
            low = mid;
        }
    }
    
    /* 线性插值 */
    v0 = p_table_points[low].voltage;
    v1 = p_table_points[high].voltage;
    t0 = p_table_points[low].temperature;
    t1 = p_table_points[high].temperature;
    
    return t0 + (voltage - v0) * (t1 - t0) / (v1 - v0);
}

/**
 * @brief  验证分度表有效性
 * @retval 0=有效, -1=无效
 */
int APP_Temp_TableVerify(void)
{
    /* 检查魔数 */
    if (p_table_header->magic != TEMP_TABLE_MAGIC)
    {
        return -1;
    }
    
    /* 检查点数 */
    if (p_table_header->point_count == 0 || 
        p_table_header->point_count > TEMP_TABLE_MAX_POINTS)
    {
        return -1;
    }
    
    return 0;
}

/**
 * @brief  获取采样计数
 * @retval 采样计数
 */
uint32_t APP_Temp_GetSampleCount(void)
{
    return g_temp.sample_count;
}
