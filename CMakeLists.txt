cmake_minimum_required(VERSION 3.15)
project(CppConcurrencyInAction
	VERSION 1.0.0
	LANGUAGES CXX
)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置构建类型（默认 Debug）
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

# 编译选项
if(MSVC)
	# MSVC 特定选项
	add_compile_options(/W4 /EHsc /permissive-)
	add_compile_options(/Zi)  # 生成调试信息
	# Debug 模式下关闭优化
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_compile_options(/Od)
	endif()
else()
	# GCC/Clang 选项
	add_compile_options(-Wall -Wextra -pedantic)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_compile_options(-g -O0)
	endif()
endif()

# 输出目录（Visual Studio 生成器会在下面创建 Debug/Release 子目录）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 添加可执行文件
add_executable(chapter01_hello_world
	src/chapter01/hello_world.cpp
)

add_executable(chapter02_managing_threads
	src/chapter02/main.cpp
)

# 添加头文件
target_sources(chapter02_managing_threads PRIVATE
	src/chapter02/wait_a_thread_to_complete.h
	src/chapter02/pass_param_to_thread.h
	src/chapter02/transfer_ownership_of_a_thread.h
	src/chapter02/joining_thread.h
)

# 多线程支持（通常自动链接，但明确指定更安全）
find_package(Threads REQUIRED)
target_link_libraries(chapter01_hello_world PRIVATE Threads::Threads)

target_link_libraries(chapter02_managing_threads PRIVATE Threads::Threads)

# 如果有其他章节的代码，可以这样组织：
# add_executable(chapter2_example
# 	src/chapter2/example.cpp
# )
# target_link_libraries(chapter2_example PRIVATE Threads::Threads)
