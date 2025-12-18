# STM32CubeIDE 工程配置步骤

## 文档信息

| 项目 | 内容 |
|------|------|
| 产品名称 | Ultra-TM02 超低温温度测量模块 |
| 文档类型 | 工程配置指南 |
| 版本 | V1.0 |
| 创建日期 | 2025年12月18日 |

---

## 一、添加包含路径（Include Paths）

### 步骤1：打开工程属性

1. 在STM32CubeIDE中，右键点击工程名称 `Ultra_TM02`
2. 选择 **Properties（属性）**

### 步骤2：进入C/C++设置

1. 在左侧菜单展开 **C/C++ Build**
2. 点击 **Settings**
3. 在右侧选择 **Tool Settings** 标签页
4. 展开 **MCU GCC Compiler**
5. 点击 **Include paths**

### 步骤3：添加包含路径

点击右侧的 **Add（添加）** 按钮（带+号的图标），依次添加以下路径：

```
../BSP/Inc
../Service/Inc
../App/Inc
```

或者使用绝对路径：
```
${workspace_loc:/${ProjName}/BSP/Inc}
${workspace_loc:/${ProjName}/Service/Inc}
${workspace_loc:/${ProjName}/App/Inc}
```

### 步骤4：确认添加完成

完成后，Include paths 列表应该包含：
- `../Core/Inc`
- `../Drivers/STM32F4xx_HAL_Driver/Inc`
- `../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy`
- `../Drivers/CMSIS/Device/ST/STM32F4xx/Include`
- `../Drivers/CMSIS/Include`
- `../USB_DEVICE/App`
- `../USB_DEVICE/Target`
- `../Middlewares/ST/STM32_USB_Device_Library/Core/Inc`
- `../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc`
- **`../BSP/Inc`** ← 新增
- **`../Service/Inc`** ← 新增
- **`../App/Inc`** ← 新增

点击 **Apply and Close（应用并关闭）**

---

## 二、添加源文件到工程

### 方法A：拖拽添加（推荐）

1. 在Windows资源管理器中打开以下文件夹：
   - `C:\c\Ultra_TM02\BSP\Src`
   - `C:\c\Ultra_TM02\Service\Src`
   - `C:\c\Ultra_TM02\App\Src`

2. 在STM32CubeIDE的Project Explorer中：
   - 展开 `Ultra_TM02` 工程
   - 展开对应的文件夹（BSP/Src、Service/Src、App/Src）

3. 将 `.c` 文件拖拽到对应的 `Src` 文件夹中

4. 在弹出的对话框中选择 **Link to files（链接到文件）** 或 **Copy files（复制文件）**

### 方法B：创建虚拟文件夹

1. 右键点击工程名称 `Ultra_TM02`
2. 选择 **New → Source Folder**
3. 输入文件夹名称（如 `BSP`）
4. 将源文件添加到对应文件夹

### 方法C：修改.cproject文件（高级）

如果工程中已存在 `.cproject` 文件，可以手动编辑添加源文件路径。

---

## 三、验证文件结构

在STM32CubeIDE的Project Explorer中，确认以下文件夹和文件存在：

```
Ultra_TM02/
├── App/
│   ├── Inc/
│   │   ├── app_comm.h
│   │   ├── app_output.h
│   │   ├── app_param.h
│   │   └── app_temp.h
│   └── Src/
│       ├── app_comm.c      ← 确认已添加
│       ├── app_output.c    ← 确认已添加
│       ├── app_param.c     ← 确认已添加
│       └── app_temp.c      ← 确认已添加
├── BSP/
│   ├── Inc/
│   │   ├── bsp_flash.h
│   │   ├── bsp_gpio.h
│   │   ├── bsp_spi.h
│   │   └── bsp_uart.h
│   └── Src/
│       ├── bsp_flash.c     ← 确认已添加
│       ├── bsp_gpio.c      ← 确认已添加
│       ├── bsp_spi.c       ← 确认已添加
│       └── bsp_uart.c      ← 确认已添加
├── Service/
│   ├── Inc/
│   │   ├── svc_adc.h
│   │   ├── svc_dac.h
│   │   ├── svc_lcd.h
│   │   └── svc_usb.h
│   └── Src/
│       ├── svc_adc.c       ← 确认已添加
│       ├── svc_dac.c       ← 确认已添加
│       ├── svc_lcd.c       ← 确认已添加
│       └── svc_usb.c       ← 确认已添加
├── Core/
│   ├── Inc/
│   └── Src/
│       └── main.c          ← 已更新
└── ...
```

---

## 四、编译工程

### 步骤1：清理工程

1. 菜单栏选择 **Project → Clean...**
2. 选择 `Ultra_TM02` 工程
3. 点击 **Clean**

### 步骤2：编译工程

1. 菜单栏选择 **Project → Build Project**
2. 或按快捷键 **Ctrl+B**
3. 或点击工具栏的锤子图标 🔨

### 步骤3：查看编译结果

在Console窗口查看编译输出：

**编译成功示例：**
```
Build Finished. 0 errors, 0 warnings. (took Xs.XXXms)
```

**如果有错误，常见问题及解决方法见下节**

---

## 五、常见编译错误及解决方法

### 错误1：找不到头文件

```
fatal error: bsp_gpio.h: No such file or directory
```

**解决方法：**
- 检查包含路径是否正确添加
- 确认路径使用 `../BSP/Inc` 格式

### 错误2：未定义的引用

```
undefined reference to `BSP_GPIO_Init'
```

**解决方法：**
- 确认 `bsp_gpio.c` 已添加到工程
- 检查文件是否被排除在编译之外（右键文件→Resource Configurations）

### 错误3：重复定义

```
multiple definition of `xxx'
```

**解决方法：**
- 检查是否有重复的源文件
- 检查头文件的 `#ifndef` 保护

### 错误4：HAL函数未声明

```
implicit declaration of function 'HAL_GPIO_WritePin'
```

**解决方法：**
- 确认 `main.h` 中包含了正确的HAL头文件
- 检查 `stm32f4xx_hal_conf.h` 中对应模块是否启用

### 错误5：USB相关错误

```
undefined reference to `CDC_Transmit_FS'
undefined reference to `hUsbDeviceFS'
```

**解决方法：**
- 确认USB_DEVICE中间件已正确配置
- 检查 `usbd_cdc_if.c` 是否存在

---

## 六、USB CDC相关配置

### 6.1 修改usbd_cdc_if.c

需要在 `USB_DEVICE/App/usbd_cdc_if.c` 中添加以下代码：

#### 添加头文件引用

在文件开头添加：
```c
/* USER CODE BEGIN INCLUDE */
#include "svc_usb.h"
/* USER CODE END INCLUDE */
```

#### 修改CDC_Receive_FS函数

找到 `CDC_Receive_FS` 函数，在其中添加回调：

```c
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
  /* 调用USB服务接收回调 */
  SVC_USB_RxCallback(Buf, *Len);
  
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  return (USBD_OK);
  /* USER CODE END 6 */
}
```

#### 添加发送状态获取函数

在文件末尾添加：
```c
/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
 * @brief  获取CDC发送状态
 * @retval 0=空闲可发送, 非0=忙
 */
uint8_t CDC_GetTxState(void)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
    if (hcdc->TxState != 0)
    {
        return 1;  /* 忙 */
    }
    return 0;  /* 空闲 */
}
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
```

#### 在头文件中声明

在 `usbd_cdc_if.h` 中添加：
```c
/* USER CODE BEGIN EXPORTED_FUNCTIONS */
uint8_t CDC_GetTxState(void);
/* USER CODE END EXPORTED_FUNCTIONS */
```

---

## 七、配置完成检查清单

请逐项确认：

- [ ] 包含路径已添加（BSP/Inc、Service/Inc、App/Inc）
- [ ] BSP层源文件已添加（4个.c文件）
- [ ] Service层源文件已添加（4个.c文件）
- [ ] App层源文件已添加（4个.c文件）
- [ ] main.c已更新
- [ ] usbd_cdc_if.c已修改（添加回调）
- [ ] 工程编译成功（0 errors）
- [ ] 警告数量合理（建议处理所有警告）

---

## 八、下载与调试

### 8.1 连接ST-Link

1. 将ST-Link V2连接到H1调试接口：
   - H1-1 (3V3) → ST-Link 3.3V（可选）
   - H1-2 (GND) → ST-Link GND
   - H1-3 (SWDIO) → ST-Link SWDIO
   - H1-4 (SWCLK) → ST-Link SWCLK

2. 将ST-Link USB连接到电脑

3. 给主板供电（24V）

### 8.2 配置调试器

1. 菜单栏 **Run → Debug Configurations...**
2. 双击 **STM32 Cortex-M C/C++ Application** 创建新配置
3. 在 **Debugger** 标签页：
   - Debug probe: ST-LINK (ST-LINK GDB server)
   - Interface: SWD
4. 点击 **Apply**

### 8.3 下载程序

1. 点击 **Debug** 按钮开始调试
2. 或使用 **Run → Run** 直接下载运行

---

*文档版本：V1.0*
*创建日期：2025年12月18日*

