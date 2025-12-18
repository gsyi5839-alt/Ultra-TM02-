@echo off
chcp 65001 >nul
echo ============================================================
echo   Ultra-TM02 代码文件复制脚本
echo ============================================================
echo.

REM 检查是否已生成CubeMX工程
if not exist "Drivers" (
    echo [错误] 请先使用STM32CubeMX生成工程!
    echo        打开 Ultra_TM02.ioc 并点击 GENERATE CODE
    pause
    exit /b 1
)

echo [1/3] 代码文件已就绪...
echo.

echo [2/3] 检查目录结构...
echo   BSP\Inc\    - %CD%\BSP\Inc\
echo   BSP\Src\    - %CD%\BSP\Src\
echo   Service\Inc\ - %CD%\Service\Inc\
echo   Service\Src\ - %CD%\Service\Src\
echo   App\Inc\    - %CD%\App\Inc\
echo   App\Src\    - %CD%\App\Src\
echo.

echo [3/3] 文件清单:
echo.
echo   BSP层:
dir /b BSP\Src\*.c 2>nul
echo.
echo   Service层:
dir /b Service\Src\*.c 2>nul
echo.
echo   App层:
dir /b App\Src\*.c 2>nul
echo.

echo ============================================================
echo 请按照 操作步骤.txt 中的说明:
echo   1. 打开Keil工程: MDK-ARM\Ultra_TM02.uvprojx
echo   2. 添加BSP、Service、App文件组
echo   3. 配置Include Paths
echo   4. 修改main.c添加初始化代码
echo   5. 编译并下载
echo ============================================================
echo.
pause

