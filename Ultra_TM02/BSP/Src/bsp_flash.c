/**
 * @file    bsp_flash.c
 * @brief   内部Flash板级支持包源文件
 * @details 实现STM32F411内部Flash读写操作，用于参数和分度表存储
 * @author  Ultra-TM02 开发团队
 * @version V1.0
 * @date    2025-12-18
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_flash.h"
#include <string.h>

/* 私有变量 ------------------------------------------------------------------*/

/* 私有函数 ------------------------------------------------------------------*/

/**
 * @brief  获取地址所在的扇区号
 * @param  addr: Flash地址
 * @retval 扇区号，-1表示无效地址
 */
static int32_t GetSector(uint32_t addr)
{
    if (addr >= 0x08000000 && addr < 0x08004000)
        return FLASH_SECTOR_0;
    else if (addr >= 0x08004000 && addr < 0x08008000)
        return FLASH_SECTOR_1;
    else if (addr >= 0x08008000 && addr < 0x0800C000)
        return FLASH_SECTOR_2;
    else if (addr >= 0x0800C000 && addr < 0x08010000)
        return FLASH_SECTOR_3;
    else if (addr >= 0x08010000 && addr < 0x08020000)
        return FLASH_SECTOR_4;
    else if (addr >= 0x08020000 && addr < 0x08040000)
        return FLASH_SECTOR_5;
    else if (addr >= 0x08040000 && addr < 0x08060000)
        return FLASH_SECTOR_6;
    else if (addr >= 0x08060000 && addr < 0x08080000)
        return FLASH_SECTOR_7;
    else
        return -1;  /* 无效地址 */
}

/* 公共函数 ------------------------------------------------------------------*/

/**
 * @brief  擦除指定扇区
 * @param  sector: 扇区号 (FLASH_SECTOR_0 - FLASH_SECTOR_7)
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_EraseSector(uint32_t sector)
{
    FLASH_EraseInitTypeDef erase_init;
    uint32_t sector_error = 0;
    HAL_StatusTypeDef status;
    
    /* 解锁Flash */
    HAL_FLASH_Unlock();
    
    /* 清除所有错误标志 */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | 
                           FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | 
                           FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    
    /* 配置擦除参数 */
    erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;  /* 2.7V-3.6V */
    erase_init.Sector = sector;
    erase_init.NbSectors = 1;
    
    /* 执行擦除 */
    status = HAL_FLASHEx_Erase(&erase_init, &sector_error);
    
    /* 锁定Flash */
    HAL_FLASH_Lock();
    
    if (status != HAL_OK)
    {
        return FLASH_ERROR_ERASE;
    }
    
    return FLASH_OK;
}

/**
 * @brief  写入数据到Flash
 * @param  addr: 目标地址（必须4字节对齐）
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度（字节数）
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_Write(uint32_t addr, uint8_t *data, uint32_t len)
{
    HAL_StatusTypeDef status;
    uint32_t i;
    uint32_t word;
    
    /* 检查地址是否有效 */
    if (GetSector(addr) < 0)
    {
        return FLASH_ERROR_ADDR;
    }
    
    /* 解锁Flash */
    HAL_FLASH_Unlock();
    
    /* 清除错误标志 */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | 
                           FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | 
                           FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    
    /* 按字（4字节）写入 */
    for (i = 0; i < len; i += 4)
    {
        /* 组合4字节为一个字 */
        word = 0;
        if (i < len) word |= data[i];
        if (i + 1 < len) word |= ((uint32_t)data[i + 1] << 8);
        if (i + 2 < len) word |= ((uint32_t)data[i + 2] << 16);
        if (i + 3 < len) word |= ((uint32_t)data[i + 3] << 24);
        
        /* 写入一个字 */
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + i, word);
        
        if (status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return FLASH_ERROR_PROGRAM;
        }
    }
    
    /* 锁定Flash */
    HAL_FLASH_Lock();
    
    /* 校验写入的数据 */
    for (i = 0; i < len; i++)
    {
        if (*((uint8_t *)(addr + i)) != data[i])
        {
            return FLASH_ERROR_VERIFY;
        }
    }
    
    return FLASH_OK;
}

/**
 * @brief  从Flash读取数据
 * @param  addr: 源地址
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度（字节数）
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_Read(uint32_t addr, uint8_t *data, uint32_t len)
{
    /* 检查地址是否有效 */
    if (GetSector(addr) < 0)
    {
        return FLASH_ERROR_ADDR;
    }
    
    /* 直接从Flash地址读取数据 */
    memcpy(data, (uint8_t *)addr, len);
    
    return FLASH_OK;
}

/**
 * @brief  擦除分度表存储区域
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_EraseTable(void)
{
    return BSP_Flash_EraseSector(FLASH_TABLE_SECTOR);
}

/**
 * @brief  擦除参数存储区域
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_EraseParam(void)
{
    return BSP_Flash_EraseSector(FLASH_PARAM_SECTOR);
}

/**
 * @brief  写入分度表数据
 * @param  offset: 相对于分度表区域起始地址的偏移
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_WriteTable(uint32_t offset, uint8_t *data, uint32_t len)
{
    uint32_t addr = FLASH_TABLE_START + offset;
    
    /* 检查是否超出分度表区域 */
    if (addr + len > FLASH_TABLE_END)
    {
        return FLASH_ERROR_ADDR;
    }
    
    return BSP_Flash_Write(addr, data, len);
}

/**
 * @brief  读取分度表数据
 * @param  offset: 相对于分度表区域起始地址的偏移
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_ReadTable(uint32_t offset, uint8_t *data, uint32_t len)
{
    uint32_t addr = FLASH_TABLE_START + offset;
    
    /* 检查是否超出分度表区域 */
    if (addr + len > FLASH_TABLE_END)
    {
        return FLASH_ERROR_ADDR;
    }
    
    return BSP_Flash_Read(addr, data, len);
}

/**
 * @brief  写入参数数据
 * @param  offset: 相对于参数区域起始地址的偏移
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_WriteParam(uint32_t offset, uint8_t *data, uint32_t len)
{
    uint32_t addr = FLASH_PARAM_START + offset;
    
    /* 检查是否超出参数区域 */
    if (addr + len > FLASH_PARAM_END)
    {
        return FLASH_ERROR_ADDR;
    }
    
    return BSP_Flash_Write(addr, data, len);
}

/**
 * @brief  读取参数数据
 * @param  offset: 相对于参数区域起始地址的偏移
 * @param  data: 数据缓冲区指针
 * @param  len: 数据长度
 * @retval Flash操作状态
 */
FlashStatus_t BSP_Flash_ReadParam(uint32_t offset, uint8_t *data, uint32_t len)
{
    uint32_t addr = FLASH_PARAM_START + offset;
    
    /* 检查是否超出参数区域 */
    if (addr + len > FLASH_PARAM_END)
    {
        return FLASH_ERROR_ADDR;
    }
    
    return BSP_Flash_Read(addr, data, len);
}
