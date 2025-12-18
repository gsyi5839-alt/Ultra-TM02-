/**
 * @file    svc_lcd.h
 * @brief   LCD服务层头文件
 * @details 提供LCD串口屏显示服务，支持淘晶驰串口屏协议
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __SVC_LCD_H
#define __SVC_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 宏定义 --------------------------------------------------------------------*/

/* LCD刷新间隔 (ms) */
#define LCD_UPDATE_INTERVAL     1000

/* 淘晶驰指令结束符 */
#define LCD_CMD_END_1           0xFF
#define LCD_CMD_END_2           0xFF
#define LCD_CMD_END_3           0xFF

/* LCD控件名称定义（根据实际界面设计修改） */
#define LCD_OBJ_TEMP            "t_temp"    /* 温度显示控件 */
#define LCD_OBJ_VOLT            "t_volt"    /* 电压显示控件 */
#define LCD_OBJ_CURR            "t_curr"    /* 输出电流显示控件 */
#define LCD_OBJ_STATUS          "t_status"  /* 状态显示控件 */
#define LCD_OBJ_SRC             "t_src"     /* 电流源显示控件 */
#define LCD_OBJ_UNIT            "t_unit"    /* 单位显示控件 */

/* 类型定义 ------------------------------------------------------------------*/

/* LCD显示数据结构 */
typedef struct {
    float temperature;      /* 温度值 (℃) */
    float voltage;          /* 电压值 (mV) */
    float output_current;   /* 输出电流 (mA) */
    uint8_t current_src;    /* 电流源选择 */
    char status[32];        /* 状态字符串 */
} LCDDisplayData_t;

/* 函数声明 ------------------------------------------------------------------*/

/**
 * @brief  LCD服务初始化
 * @retval 无
 */
void SVC_LCD_Init(void);

/**
 * @brief  LCD周期更新（在主循环中调用）
 * @retval 无
 */
void SVC_LCD_Update(void);

/**
 * @brief  设置温度显示值
 * @param  temp: 温度值 (℃)
 * @retval 无
 */
void SVC_LCD_SetTemperature(float temp);

/**
 * @brief  设置电压显示值
 * @param  voltage: 电压值 (mV)
 * @retval 无
 */
void SVC_LCD_SetVoltage(float voltage);

/**
 * @brief  设置输出电流显示值
 * @param  current: 电流值 (mA)
 * @retval 无
 */
void SVC_LCD_SetCurrent(float current);

/**
 * @brief  设置状态显示
 * @param  status: 状态字符串
 * @retval 无
 */
void SVC_LCD_SetStatus(const char *status);

/**
 * @brief  设置电流源显示
 * @param  src: 电流源选择 (0=10μA, 1=17μA)
 * @retval 无
 */
void SVC_LCD_SetCurrentSource(uint8_t src);

/**
 * @brief  发送原始指令到LCD
 * @param  cmd: 指令字符串
 * @retval 无
 */
void SVC_LCD_SendCommand(const char *cmd);

/**
 * @brief  设置控件文本
 * @param  obj_name: 控件名称
 * @param  text: 文本内容
 * @retval 无
 */
void SVC_LCD_SetText(const char *obj_name, const char *text);

/**
 * @brief  设置控件数值
 * @param  obj_name: 控件名称
 * @param  value: 数值
 * @retval 无
 */
void SVC_LCD_SetValue(const char *obj_name, int value);

/**
 * @brief  切换LCD页面
 * @param  page: 页面编号
 * @retval 无
 */
void SVC_LCD_SetPage(uint8_t page);

/**
 * @brief  强制立即刷新显示
 * @retval 无
 */
void SVC_LCD_Refresh(void);

#ifdef __cplusplus
}
#endif

#endif /* __SVC_LCD_H */
