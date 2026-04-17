# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 在此仓库中工作时提供指引。

## 项目概览

本仓库用于学习《C++ Concurrency in Action (2nd Edition)》，包含配套的动手代码示例。项目使用 C++20 特性，通过实践实现来学习并发编程模式。

## 构建与开发命令

项目使用 **CMake Presets**（`CMakePresets.json`）做跨平台构建。VS Code 的 CMake Tools 扩展会自动识别 preset。

### macOS
```bash
# 配置 + 构建（Debug）
cmake --preset macos-debug
cmake --build --preset macos-debug

# Release
cmake --preset macos-release
cmake --build --preset macos-release

# 构建指定 target
cmake --build --preset macos-debug --target chapter02_managing_threads

# 运行
./bin/chapter01_hello_world
./bin/chapter02_managing_threads
./bin/chapter03_sharing_data
```

### Windows
```powershell
# 配置 + 构建（Debug）
cmake --preset windows-debug
cmake --build --preset windows-debug

# Release
cmake --preset windows-release
cmake --build --preset windows-release

# 构建指定 target
cmake --build --preset windows-debug --target chapter02_managing_threads

# 运行
.\bin\Debug\chapter01_hello_world.exe
.\bin\Debug\chapter02_managing_threads.exe
.\bin\Debug\chapter03_sharing_data.exe
```

### Visual Studio 集成（仅 Windows）
```powershell
start build\windows-debug\CppConcurrencyInAction.sln
.\open_vs.bat
```

## 项目架构

### 按章节组织

代码按书本章节组织，每章包含若干基于头文件的示例：

```
src/
├── chapter01/          # 入门
│   └── hello_world.cpp
├── chapter02/          # 线程管理
│   ├── main.cpp        # 入口：一次只 include 一个示例
│   ├── 2.1_thread_guard.h
│   ├── 2.2_pass_params.h
│   ├── 2.3_scoped_thread.h
│   ├── 2.3_joining_thread.h
│   ├── 2.4_batch_threads.h
│   └── 2.4_multi_thread_accumulate.h
├── chapter03/          # 线程间共享数据
│   ├── main.cpp
│   ├── 3.1_data_race.h
│   └── 3.2_mutex.h
└── chapterNN/          # 后续章节……
```

### 示例切换约定

每章的 `main.cpp` 一次只 include 一个示例头文件：

```cpp
// 取消注释其中一行来测试对应示例
// #include "wait_a_thread_to_complete.h"
// #include "pass_param_to_thread.h"
#include "joining_thread.h"  // 当前激活

int main() {
    test();  // 每个头文件提供一个 test() 函数
}
```

**新增示例的步骤：**
1. 在对应章节目录下创建新的 `.h` 文件
2. 在其中实现示例，提供 `test()` 或类似函数
3. 注释掉 `main.cpp` 中其他 include，引入新头文件
4. 在 `CMakeLists.txt` 的 `target_sources()` 里登记该头文件

### 新增章节

以新增第四章为例：

1. 创建目录：`src/chapter04/`
2. 在 `CMakeLists.txt` 中追加：
```cmake
add_executable(chapter04_example
    src/chapter04/main.cpp
)

target_sources(chapter04_example PRIVATE
    src/chapter04/example1.h
    src/chapter04/example2.h
)

target_link_libraries(chapter04_example PRIVATE Threads::Threads)
```
3. 在 `.vscode/launch.json` 中复制一份现有章节配置，修改 `name` 和 `program`：
```json
{
    "name": "Debug chapter04 (example)",
    "type": "lldb",
    "request": "launch",
    "program": "${workspaceFolder}/bin/chapter04_example",
    "args": [],
    "cwd": "${workspaceFolder}",
    "preLaunchTask": "Build (Debug)"
}
```

## 构建系统细节

### CMake Presets

跨平台构建由 `CMakePresets.json` 统一管理：
- **Windows**：Visual Studio 18 2026 生成器，x64
- **macOS**：Ninja 生成器
- **Linux**：Ninja 生成器

每个平台都有 Debug 与 Release preset，借助 `condition` 字段只在匹配的 OS 上显示。

各 preset 的 build 目录已按名称拆分（`build/macos-debug`、`build/macos-release`、`build/windows-debug` 等），Debug 与 Release 可以共存，不需要 `rm -rf`。最终可执行文件统一通过 `CMAKE_RUNTIME_OUTPUT_DIRECTORY` 输出到 `bin/`，所以 `launch.json` 中的路径始终稳定。

### 编译选项

- **标准**：C++20（`CMAKE_CXX_STANDARD 20`）
- **MSVC**：`/W4 /EHsc /permissive-` + `/Zi`（生成调试信息）
- **GCC/Clang**：`-Wall -Wextra -pedantic`
- **Debug**：MSVC 用 `/Od`，GCC/Clang 用 `-g -O0`
- **输出目录**：`bin/`（macOS/Linux），`bin/Debug/` 或 `bin/Release/`（Windows + VS 生成器）

### 多线程支持

所有 target 都链接 `Threads::Threads`：
```cmake
find_package(Threads REQUIRED)
target_link_libraries(chapter01_hello_world PRIVATE Threads::Threads)
```

## VS Code 调试（macOS/Linux）

按 F5 会通过 **CodeLLDB** 扩展启动 `.vscode/launch.json` 中当前选中的配置。

- 需安装 **CodeLLDB** 扩展。每章各有一个调试配置——在"运行和调试"下拉中选择。
- `preLaunchTask` 指向 `.vscode/tasks.json` 中的 shell task：`Build (Debug)`、`Configure (Debug)`、`Clean (Debug)`。它们直接调用 `cmake --preset <os>-debug`，不依赖 CMake Tools 扩展的"激活 preset"状态。
- **标签冲突提醒**：CMake Tools 扩展会注册自己的 `CMake: Build / Clean / Configure` 命令，这些命令由扩展内部的"激活 preset"驱动（可能过期或指向不存在的 build 目录）。通过 Command Palette → "Tasks: Run Task" 执行时，请选不带 `CMake:` 前缀的那几个标签。
- **macOS 上的调试符号**：Apple Clang 默认把 DWARF 留在 `.o` 文件里，可执行文件中只保留一张 debug map。即使 `bin/` 还在，只要删了 `build/macos-debug/`，LLDB 就无法调试——需要重新构建。

### 多线程调试小贴士

- LLDB 默认 **all-stop** 模式：任一线程命中断点，所有线程一起暂停。CALL STACK 面板列出所有线程，点击可把 Variables 面板切换到对应线程上下文。
- 对高频断点（紧循环、多线程）：右键断点 → Edit → 加 Hit Count 或条件表达式，避免被命中淹没。
- 调试器暂停会扭曲调度。**不要**从断点观察推断运行时线程交替——某个线程"总是先命中"可能只是因为 all-stop 让它的缓存保持热；真实运行未必如此。观察调度行为请用 `std::cout` 打日志。
- 想只让某个线程单步、其他保持暂停：Debug Console → `thread continue <tid>`（tid 取自 CALL STACK）。

## Visual Studio 调试（仅 Windows）

并发代码常用的调试工具：

- **Threads 窗口**：查看所有线程的 ID 和状态
- **Parallel Stacks**：可视化各线程调用栈
- **Parallel Tasks**：监控并发任务
- **Diagnostics Tools**：线程活动时间线

详细调试流程见 `docs/build_and_debug_vs.md`。

## 文档

- **`docs/plan.md`**：10–12 周的学习计划
- **`docs/study_notes.md`**：按章节整理的学习笔记，含示例
- **`docs/build_and_debug_vs.md`**：Visual Studio 完整调试流程
- **`study_materials/`**：《C++ Concurrency in Action 2nd Edition》中译材料

## 重要约束

- **平台**：Windows (MSVC) + macOS (Apple Clang) + Linux (GCC/Clang)
- **换行符**：由 `.gitattributes` 自动管理（`* text=auto`）
- **编码**：UTF-8
- **缩进**：Tab（宽度 4）
- **无测试**：这是学习项目，示例手动运行
- **单示例测试**：每章一次只跑一个示例
