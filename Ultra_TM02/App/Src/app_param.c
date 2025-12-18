/**
 * @file    app_param.c
 * @brief   参数管理应用层源文件
 * @details 实现参数存储、加载、管理功能
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "app_param.h"
#include "bsp_flash.h"
#include <string.h>

/* 私有变量 ------------------------------------------------------------------*/

/* 用户参数（RAM中的副本） */
static UserParam_t g_param = {
    .magic = PARAM_MAGIC,
    .version = PARAM_VERSION,
    .reserved = 0,
    .current_source = DEFAULT_CURRENT_SOURCE,
    .current_adj_10uA = DEFAULT_CURRENT_ADJ_10,
    .current_adj_17uA = DEFAULT_CURRENT_ADJ_17,
    .temp_4mA = DEFAULT_TEMP_4MA,
    .temp_20mA = DEFAULT_TEMP_20MA,
    .crc = 0
};

/* 私有函数声明 --------------------------------------------------------------*/
static uint16_t CalcParamCRC(UserParam_t *param);
static int VerifyParam(UserParam_t *param);

/* 私有函数 ------------------------------------------------------------------*/

/**
 * @brief  计算参数CRC16校验值
 * @param  param: 参数结构体指针
 * @retval CRC16值
 */
static uint16_t CalcParamCRC(UserParam_t *param)
{
    uint16_t crc = 0xFFFF;
    uint8_t *data = (uint8_t *)param;
    uint16_t len = sizeof(UserParam_t) - sizeof(uint16_t) * 2;  /* 不包含crc和padding2 */
    
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

/**
 * @brief  验证参数有效性
 * @param  param: 参数结构体指针
 * @retval 0=有效, -1=无效
 */
static int VerifyParam(UserParam_t *param)
{
    /* 检查魔数 */
    if (param->magic != PARAM_MAGIC)
    {
        return -1;
    }
    
    /* 检查版本（可以兼容低版本） */
    if (param->version > PARAM_VERSION)
    {
        return -1;
    }
    
    /* 检查CRC */
    if (param->crc != CalcParamCRC(param))
    {
        return -1;
    }
    
    /* 检查参数范围 */
    if (param->current_source > 1)
    {
        return -1;
    }
    
    return 0;
}

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @brief  参数模块初始化
 * @retval 无
 */
void APP_Param_Init(void)
{
    /* 尝试加载参数 */
    if (APP_Param_Load() != 0)
    {
        /* 加载失败，使用默认值 */
        APP_Param_SetDefault();
    }
}

/**
 * @brief  加载参数
 * @retval 0=成功, -1=失败（使用默认值）
 */
int APP_Param_Load(void)
{
    UserParam_t temp_param;
    FlashStatus_t status;
    
    /* 从Flash读取参数 */
    status = BSP_Flash_ReadParam(0, (uint8_t *)&temp_param, sizeof(UserParam_t));
    
    if (status != FLASH_OK)
    {
        return -1;
    }
    
    /* 验证参数 */
    if (VerifyParam(&temp_param) != 0)
    {
        return -1;
    }
    
    /* 复制到RAM */
    memcpy(&g_param, &temp_param, sizeof(UserParam_t));
    
    return 0;
}

/**
 * @brief  保存参数
 * @retval 0=成功, -1=失败
 */
int APP_Param_Save(void)
{
    FlashStatus_t status;
    
    /* 更新CRC */
    g_param.crc = CalcParamCRC(&g_param);
    
    /* 擦除参数区域 */
    status = BSP_Flash_EraseParam();
    if (status != FLASH_OK)
    {
        return -1;
    }
    
    /* 写入参数 */
    status = BSP_Flash_WriteParam(0, (uint8_t *)&g_param, sizeof(UserParam_t));
    if (status != FLASH_OK)
    {
        return -1;
    }
    
    return 0;
}

/**
 * @brief  恢复默认参数
 * @retval 无
 */
void APP_Param_SetDefault(void)
{
    g_param.magic = PARAM_MAGIC;
    g_param.version = PARAM_VERSION;
    g_param.reserved = 0;
    g_param.current_source = DEFAULT_CURRENT_SOURCE;
    g_param.current_adj_10uA = DEFAULT_CURRENT_ADJ_10;
    g_param.current_adj_17uA = DEFAULT_CURRENT_ADJ_17;
    g_param.temp_4mA = DEFAULT_TEMP_4MA;
    g_param.temp_20mA = DEFAULT_TEMP_20MA;
    g_param.crc = CalcParamCRC(&g_param);
}

/**
 * @brief  获取电流源选择
 * @retval 电流源 (0:10μA, 1:17μA)
 */
uint8_t APP_Param_GetCurrentSource(void)
{
    return g_param.current_source;
}

/**
 * @brief  设置电流源选择
 * @param  src: 电流源 (0:10μA, 1:17μA)
 * @retval 无
 */
void APP_Param_SetCurrentSource(uint8_t src)
{
    if (src <= 1)
    {
        g_param.current_source = src;
    }
}

/**
 * @brief  获取10μA调整值
 * @retval 调整值 (μA)
 */
float APP_Param_GetCurrentAdj10(void)
{
    return g_param.current_adj_10uA;
}

/**
 * @brief  设置10μA调整值
 * @param  adj: 调整值 (μA)
 * @retval 无
 */
void APP_Param_SetCurrentAdj10(float adj)
{
    /* 限制范围 ±1μA */
    if (adj > 1.0f) adj = 1.0f;
    if (adj < -1.0f) adj = -1.0f;
    
    g_param.current_adj_10uA = adj;
}

/**
 * @brief  获取17μA调整值
 * @retval 调整值 (μA)
 */
float APP_Param_GetCurrentAdj17(void)
{
    return g_param.current_adj_17uA;
}

/**
 * @brief  设置17μA调整值
 * @param  adj: 调整值 (μA)
 * @retval 无
 */
void APP_Param_SetCurrentAdj17(float adj)
{
    /* 限制范围 ±1μA */
    if (adj > 1.0f) adj = 1.0f;
    if (adj < -1.0f) adj = -1.0f;
    
    g_param.current_adj_17uA = adj;
}

/**
 * @brief  获取4mA对应温度
 * @retval 温度值 (℃)
 */
float APP_Param_Get4mATemp(void)
{
    return g_param.temp_4mA;
}

/**
 * @brief  设置4mA对应温度
 * @param  temp: 温度值 (℃)
 * @retval 无
 */
void APP_Param_Set4mATemp(float temp)
{
    g_param.temp_4mA = temp;
}

/**
 * @brief  获取20mA对应温度
 * @retval 温度值 (℃)
 */
float APP_Param_Get20mATemp(void)
{
    return g_param.temp_20mA;
}

/**
 * @brief  设置20mA对应温度
 * @param  temp: 温度值 (℃)
 * @retval 无
 */
void APP_Param_Set20mATemp(float temp)
{
    g_param.temp_20mA = temp;
}

/**
 * @brief  获取参数结构体指针
 * @retval 参数结构体指针
 */
UserParam_t* APP_Param_GetStruct(void)
{
    return &g_param;
}
