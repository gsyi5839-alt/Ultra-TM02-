/**
 * @file    svc_lcd.c
 * @brief   LCD服务层源文件
 * @details 实现LCD串口屏显示服务，支持淘晶驰串口屏协议
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "svc_lcd.h"
#include "bsp_uart.h"
#include <stdio.h>
#include <string.h>

/* 私有变量 ------------------------------------------------------------------*/

/* 显示数据 */
static LCDDisplayData_t lcd_data = {
    .temperature = 0.0f,
    .voltage = 0.0f,
    .output_current = 4.0f,
    .current_src = 0,
    .status = "Ready"
};

/* 上次更新时间 */
static uint32_t last_update_tick = 0;

/* 数据更新标志 */
static uint8_t data_changed = 0;

/* 私有函数 ------------------------------------------------------------------*/

/**
 * @brief  发送指令结束符
 * @retval 无
 */
static void LCD_SendEnd(void)
{
    uint8_t end_bytes[3] = {LCD_CMD_END_1, LCD_CMD_END_2, LCD_CMD_END_3};
    BSP_UART_Transmit(end_bytes, 3);
}

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @brief  LCD服务初始化
 * @retval 无
 */
void SVC_LCD_Init(void)
{
    /* 初始化UART */
    BSP_UART_Init();
    
    /* 等待LCD上电稳定 */
    HAL_Delay(500);
    
    /* 发送复位指令 */
    SVC_LCD_SendCommand("rest");
    HAL_Delay(500);
    
    /* 切换到主页面 */
    SVC_LCD_SetPage(0);
    
    /* 设置初始显示 */
    SVC_LCD_SetStatus("Initializing...");
    SVC_LCD_SetTemperature(0.0f);
    SVC_LCD_SetVoltage(0.0f);
    SVC_LCD_SetCurrent(4.0f);
    SVC_LCD_SetCurrentSource(0);
    
    /* 更新时间戳 */
    last_update_tick = HAL_GetTick();
}

/**
 * @brief  LCD周期更新（在主循环中调用）
 * @note   以LCD_UPDATE_INTERVAL间隔更新显示
 * @retval 无
 */
void SVC_LCD_Update(void)
{
    uint32_t current_tick = HAL_GetTick();
    char buf[64];
    
    /* 检查更新间隔 */
    if (current_tick - last_update_tick < LCD_UPDATE_INTERVAL)
    {
        return;
    }
    
    last_update_tick = current_tick;
    
    /* 更新温度显示（保留3位小数） */
    snprintf(buf, sizeof(buf), "%.3f", lcd_data.temperature);
    SVC_LCD_SetText(LCD_OBJ_TEMP, buf);
    
    /* 更新电压显示（保留3位小数） */
    snprintf(buf, sizeof(buf), "%.3f", lcd_data.voltage);
    SVC_LCD_SetText(LCD_OBJ_VOLT, buf);
    
    /* 更新输出电流显示（保留2位小数） */
    snprintf(buf, sizeof(buf), "%.2f", lcd_data.output_current);
    SVC_LCD_SetText(LCD_OBJ_CURR, buf);
    
    /* 更新电流源显示 */
    if (lcd_data.current_src == 0)
    {
        SVC_LCD_SetText(LCD_OBJ_SRC, "10uA");
    }
    else
    {
        SVC_LCD_SetText(LCD_OBJ_SRC, "17uA");
    }
    
    /* 更新状态显示 */
    SVC_LCD_SetText(LCD_OBJ_STATUS, lcd_data.status);
    
    /* 清除更新标志 */
    data_changed = 0;
}

/**
 * @brief  设置温度显示值
 * @param  temp: 温度值 (℃)
 * @retval 无
 */
void SVC_LCD_SetTemperature(float temp)
{
    lcd_data.temperature = temp;
    data_changed = 1;
}

/**
 * @brief  设置电压显示值
 * @param  voltage: 电压值 (mV)
 * @retval 无
 */
void SVC_LCD_SetVoltage(float voltage)
{
    lcd_data.voltage = voltage;
    data_changed = 1;
}

/**
 * @brief  设置输出电流显示值
 * @param  current: 电流值 (mA)
 * @retval 无
 */
void SVC_LCD_SetCurrent(float current)
{
    lcd_data.output_current = current;
    data_changed = 1;
}

/**
 * @brief  设置状态显示
 * @param  status: 状态字符串
 * @retval 无
 */
void SVC_LCD_SetStatus(const char *status)
{
    strncpy(lcd_data.status, status, sizeof(lcd_data.status) - 1);
    lcd_data.status[sizeof(lcd_data.status) - 1] = '\0';
    data_changed = 1;
}

/**
 * @brief  设置电流源显示
 * @param  src: 电流源选择 (0=10μA, 1=17μA)
 * @retval 无
 */
void SVC_LCD_SetCurrentSource(uint8_t src)
{
    lcd_data.current_src = src;
    data_changed = 1;
}

/**
 * @brief  发送原始指令到LCD
 * @param  cmd: 指令字符串
 * @retval 无
 */
void SVC_LCD_SendCommand(const char *cmd)
{
    BSP_UART_SendString(cmd);
    LCD_SendEnd();
}

/**
 * @brief  设置控件文本
 * @param  obj_name: 控件名称
 * @param  text: 文本内容
 * @retval 无
 */
void SVC_LCD_SetText(const char *obj_name, const char *text)
{
    char cmd[128];
    
    /* 格式: obj_name.txt="text" */
    snprintf(cmd, sizeof(cmd), "%s.txt=\"%s\"", obj_name, text);
    SVC_LCD_SendCommand(cmd);
}

/**
 * @brief  设置控件数值
 * @param  obj_name: 控件名称
 * @param  value: 数值
 * @retval 无
 */
void SVC_LCD_SetValue(const char *obj_name, int value)
{
    char cmd[64];
    
    /* 格式: obj_name.val=value */
    snprintf(cmd, sizeof(cmd), "%s.val=%d", obj_name, value);
    SVC_LCD_SendCommand(cmd);
}

/**
 * @brief  切换LCD页面
 * @param  page: 页面编号
 * @retval 无
 */
void SVC_LCD_SetPage(uint8_t page)
{
    char cmd[16];
    
    /* 格式: page n */
    snprintf(cmd, sizeof(cmd), "page %d", page);
    SVC_LCD_SendCommand(cmd);
}

/**
 * @brief  强制立即刷新显示
 * @retval 无
 */
void SVC_LCD_Refresh(void)
{
    /* 重置更新时间，强制下次Update时立即刷新 */
    last_update_tick = HAL_GetTick() - LCD_UPDATE_INTERVAL;
    SVC_LCD_Update();
}
