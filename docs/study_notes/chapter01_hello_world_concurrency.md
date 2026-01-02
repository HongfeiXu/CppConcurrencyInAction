# Chapter 1: Hello, world of concurrency in C++!

## 主要内容

- 定义并发和多线程
- 使用并发和多线程
- C++的并发史
- 简单的C++多线程

## `std::thread` 析构规则

析构前必须已 join 或 detach，否则析构函数会调用 std::terminate()。

abort程序示例：

```cpp
#include <thread>
#include <iostream>

int main()
{
	std::thread t([](){
		std::cout << "Hello from thread" << std::endl;
	});
	// t.join();
	return 0;
}

// 在 t 析构之前，必须调用：
// - t.join()   // 等待线程完成
// - t.detach() // 分离线程（后台运行）
// 否则会导致 std::terminate() -> abort()
```

>进一步，思考这个规则的原因。
