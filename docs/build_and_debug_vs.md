# 编译与调试方案（Visual Studio 版本）

本文档记录使用 **Visual Studio 进行编译和调试**的工作流程。

## 工具链配置

### 核心工具
- **编辑器**: Cursor - AI 辅助编程
- **编译器**: MSVC (Visual Studio 2026 Community)
- **构建系统**: CMake (>= 3.15)
- **调试器**: Visual Studio Debugger
- **IDE**: Visual Studio 2026 - 编译和调试
- **C++ 标准**: C++20

### Cursor 扩展
- **C/C++** (Anysphere) - IntelliSense
- **clangd**
- **CMake Tools** (可选) - CMake 集成

### 配置文件
- `.vscode/settings.json` - 工作区设置（Tab 缩进、UTF-8、CRLF）
- `.vscode/tasks.json` - 构建任务（CMake、打开 VS）
- `CMakeLists.txt` - 项目构建配置

## 工作流程

### 1. 在 Cursor 中编写代码

使用 Cursor 的优势：
- AI 辅助编程（Composer, Chat）
- 语法高亮和代码补全
- Git 集成
- 快速编辑

### 2. 生成 Visual Studio 解决方案

**方法 A: 使用 CMake Tools（推荐）**
1. 在 Cursor 中打开命令面板 (`Ctrl + Shift + P`)
2. 选择 `CMake: Configure`
3. 选择生成器：`Visual Studio 18 2026`
4. 生成完成后，`.sln` 文件在 `build/` 目录

**方法 B: 使用任务**
1. `Ctrl + Shift + P` → `Tasks: Run Task`
2. 选择 `CMake: configure`

**方法 C: 命令行**
```powershell
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
```

### 3. 在 Visual Studio 中打开解决方案

**方法 A: 双击打开**
- 直接双击 `build/CppConcurrencyInAction.sln`

**方法 B: 从 Cursor 打开**
- 在文件资源管理器中右键 `.sln` → "Open with Visual Studio"

**方法 C: 使用脚本（见下文）**
- 双击 `open_vs.bat`

### 4. 在 Visual Studio 中编译

1. 打开解决方案后，选择配置：
   - Debug 或 Release（顶部工具栏）
   - x64 平台（旁边下拉框）

2. 编译：
   - `Ctrl + Shift + B` 或
   - 菜单：`生成` → `生成解决方案`

3. 查看输出：
   - 可执行文件在 `bin/Debug/chapter01_hello_world.exe` 或 `bin/Release/chapter01_hello_world.exe`（根据 CMakeLists.txt 中的目标名称）

### 5. 在 Visual Studio 中调试

1. **设置断点**
   - 点击代码行号左侧，或按 `F9`

2. **启动调试**
   - `F5` - 开始调试
   - `Ctrl + F5` - 开始执行（不调试）

3. **调试功能**
   - **单步执行**：
     - `F10` - Step Over
     - `F11` - Step Into
     - `Shift + F11` - Step Out

   - **多线程调试**：
     - `调试` → `窗口` → `线程`
     - `调试` → `窗口` → `并行堆栈`
     - 查看所有线程，切换线程上下文

   - **变量查看**：
     - `调试` → `窗口` → `局部变量`
     - `调试` → `窗口` → `监视`（可添加表达式）
     - `调试` → `窗口` → `调用堆栈`

4. **并发编程专用工具**
   - **并发可视化工具**：
     - `调试` → `窗口` → `并行任务`
     - `调试` → `窗口` → `GPU 线程`

   - **数据竞争检测**：
     - `调试` → `窗口` → `诊断工具`
     - 启用 `并发` 分析

## 快捷方式配置

### 创建 Visual Studio 启动脚本

创建 `open_vs.bat`（项目根目录）：

```batch
@echo off
start "" "C:\Program Files\Microsoft Visual Studio\2026\Community\Common7\IDE\devenv.exe" "%~dp0build\CppConcurrencyInAction.sln"
```

使用方法：
- 双击 `open_vs.bat`

### 使用 VS Code 任务（已配置）

在 `.vscode/tasks.json` 中已配置以下任务：

- **CMake: configure** - 配置 CMake 项目
- **CMake: build (Debug)** - Debug 构建（默认任务 `Ctrl + Shift + B`）
- **CMake: build (Release)** - Release 构建
- **CMake: clean** - 清理构建文件
- **Open Visual Studio** - 打开 Visual Studio 解决方案

使用方法：`Ctrl + Shift + P` → `Tasks: Run Task` → 选择任务

## Visual Studio 调试技巧

### 多线程调试

1. **查看所有线程**
   - 调试时：`调试` → `窗口` → `线程`
   - 显示所有线程、线程 ID、状态、位置

2. **切换线程上下文**
   - 双击线程窗口中的线程
   - 查看该线程的局部变量和调用栈

3. **并行堆栈视图**
   - `调试` → `窗口` → `并行堆栈`
   - 可视化所有线程的调用栈关系

4. **线程断点**
   - 设置断点 → 右键 → `筛选器`
   - 可以设置条件，如：`ThreadName == "WorkerThread"`

### 条件断点和日志断点

1. **条件断点**
   - 设置断点 → 右键 → `条件`
   - 例如：`i > 10` 或 `thread_id == 2`

2. **命中计数断点**
   - 右键断点 → `命中计数`
   - 例如：在第 5 次命中时停止

3. **操作断点（日志）**
   - 右键断点 → `操作`
   - 勾选"记录消息"，例如：`Value of i: {i}`

### 监视和即时窗口

1. **监视窗口**
   - 可以添加任意表达式
   - 例如：`std::this_thread::get_id()`

2. **即时窗口**
   - `调试` → `窗口` → `即时`
   - 可以执行 C++ 表达式，调用函数

### 并发诊断工具

1. **诊断工具**
   - `调试` → `窗口` → `诊断工具`
   - 启用"并发"分析
   - 查看线程活动时间线

2. **性能分析器**
   - `调试` → `性能探查器`
   - 可以分析并发性能、CPU 使用率等

## 常见问题

### Q: CMakeLists.txt 修改后需要做什么？

**A**:
1. 在 Cursor 中重新运行 `CMake: Configure`
2. Visual Studio 会提示重新加载项目，点击"重新加载"

