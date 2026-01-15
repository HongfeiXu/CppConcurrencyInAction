@echo off
REM Open Visual Studio Solution
REM 用法: open_vs.bat

set "SLN_PATH=%~dp0build\CppConcurrencyInAction.slnx"
set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2026\Community\Common7\IDE\devenv.exe"

if not exist "%SLN_PATH%" (
    echo 错误: 找不到解决方案文件: %SLN_PATH%
    echo 请先运行 CMake: Configure 生成解决方案
    pause
    exit /b 1
)

if not exist "%VS_PATH%" (
    echo 错误: 找不到 Visual Studio: %VS_PATH%
    echo 请检查 Visual Studio 2026 是否已安装
    pause
    exit /b 1
)

echo 正在打开 Visual Studio...
start "" "%VS_PATH%" "%SLN_PATH%"

