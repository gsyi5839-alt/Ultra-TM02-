/**
 * @file    bsp_flash.h
 * @brief   内部Flash板级支持包头文件
 * @details 提供STM32F411内部Flash读写操作，用于参数和分度表存储
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"

/* 宏定义 --------------------------------------------------------------------*/

/**
 * STM32F411RET6 Flash布局 (512KB)
 * 
 * Sector 0:  0x08000000 - 0x08003FFF (16KB)  - 程序代码
 * Sector 1:  0x08004000 - 0x08007FFF (16KB)  - 程序代码
 * Sector 2:  0x08008000 - 0x0800BFFF (16KB)  - 程序代码
 * Sector 3:  0x0800C000 - 0x0800FFFF (16KB)  - 程序代码
 * Sector 4:  0x08010000 - 0x0801FFFF (64KB)  - 程序代码
 * Sector 5:  0x08020000 - 0x0803FFFF (128KB) - 程序代码
 * Sector 6:  0x08040000 - 0x0805FFFF (128KB) - 分度表存储
 * Sector 7:  0x08060000 - 0x0807FFFF (128KB) - 用户参数
 */

/* 程序代码区域 */
#define FLASH_CODE_START        0x08000000
#define FLASH_CODE_END          0x0803FFFF

/* 分度表存储区域 (Sector 6) */
#define FLASH_TABLE_START       0x08040000
#define FLASH_TABLE_END         0x0805FFFF
#define FLASH_TABLE_SIZE        (128 * 1024)  /* 128KB */
#define FLASH_TABLE_SECTOR      FLASH_SECTOR_6

/* 用户参数存储区域 (Sector 7) */
#define FLASH_PARAM_START       0x08060000
#define FLASH_PARAM_END         0x0807FFFF
#define FLASH_PARAM_SIZE        (128 * 1024)  /* 128KB */
#define FLASH_PARAM_SECTOR      FLASH_SECTOR_7

/* 类型定义 ------------------------------------------------------------------*/

/* Flash操作状态 */
typedef enum {
    FLASH_OK = 0,           /* 操作成功 */
    FLASH_ERROR_ERASE,      /* 擦除错误 */
    FLASH_ERROR_PROGRAM,    /* 编程错误 */
    FLASH_ERROR_VERIFY,     /* 校验错误 */
    FLASH_ERROR_ADDR,       /* 地址错误 */
    FLASH_ERROR_BUSY        /* Flash忙 */
} FlashStatus_t;

/* 函数声明 ------------------------------------------------------------------*/

/**
 * @brief  擦除指定扇区
 * @param  sector: 扇区号 (FLASH_SECTOR_0 - FLASH_SECTOR_7)
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_EraseSector(uint32_t sector);

/**
 * @brief  写入数据到Flash
 * @param  addr: 目标地址（必须4字节对齐）
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度（字节数）
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_Write(uint32_t addr, uint8_t *data, uint32_t len);

/**
 * @brief  从Flash读取数据
 * @param  addr: 源地址
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度（字节数）
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_Read(uint32_t addr, uint8_t *data, uint32_t len);

/**
 * @brief  擦除分度表存储区域
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_EraseTable(void);

/**
 * @brief  擦除参数存储区域
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_EraseParam(void);

/**
 * @brief  写入分度表数据
 * @param  offset: 相对于分度表区域起始地址的偏移
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_WriteTable(uint32_t offset, uint8_t *data, uint32_t len);

/**
 * @brief  读取分度表数据
 * @param  offset: 相对于分度表区域起始地址的偏移
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_ReadTable(uint32_t offset, uint8_t *data, uint32_t len);

/**
 * @brief  写入参数数据
 * @param  offset: 相对于参数区域起始地址的偏移
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_WriteParam(uint32_t offset, uint8_t *data, uint32_t len);

/**
 * @brief  读取参数数据
 * @param  offset: 相对于参数区域起始地址的偏移
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_ReadParam(uint32_t offset, uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_FLASH_H */
