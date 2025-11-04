# 基础入门

## Chapter 1: Hello, world of concurrency in C++!

### 主要内容

- 定义并发和多线程
- 使用并发和多线程
- C++的并发史
- 简单的C++多线程

### `std::thread` 析构规则

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


## Chapter 2: Managling threads

### 主要内容

- 启动新线程
- 等待与分离
- 唯一标识符

### 线程的基本操作

等待线程完成

- Using RAII to wait for a thread to complete

```cpp
class thread_guard
{
	std::thread& t;
public:
	explicit thread_guard(std::thread& t_): t(t_) {}

	~thread_guard()
	{
		if(t.joinable())
		{
			t.join();
		}
	}

	thread_guard(thread_guard const&)=delete; // 禁用拷贝构造
	thread_guard& operator=(thread_guard const&)=delete; // 禁用拷贝赋值操作符
};
```

后台运行线程

- detach() 会让线程在后台运行，与主线程不能直接交互
- C++运行库保证，当线程退出时，相关资源能够正确回收
- 不能对没有执行线程的 std::thread 对象使用 detach()
- 调用 detach() 之前，类似调用 join() 之前，需要先判断是否 joinable

### 传递参数

- 简单例子：

```cpp
void f(int i, std::string const& s);
std::thread t(f, 3, "hello")
```

- 参数通过值传递：std::thread 构造函数会移动或拷贝参数，不会直接按引用传递。
- 需要引用时需包装：要用引用传递时，用 std::ref（可修改）或 std::cref（只读）包装
- 见如下例子：

```cpp
void pass_reference_to_thread(widget_id w)
{
	widget_data data;
	// data 会被拷贝，以右值的方式传递给 update_data_for_widget，编译出错
	// std::thread t(update_data_for_widget, w, data);

	// data 会被以引用的形式传递给 update_data_for_widget，编译通过
	std::thread t(update_data_for_widget, w, std::ref(data));
	t.join();
}
```

- 调用对象的成员函数作为线程函数
- 此时，std::thread 构造函数的第3个参数，就是成员函数的第1个参数，以此类推
- 见如下例子

```cpp
class X
{
public:
	void do_lengthy_work(int i)
	{
		std::cout << "do_lengthy_work(" << i << ")" << std::endl;
	}
};

void call_member_function_as_thread_function()
{
	X my_x;
	std::thread t(&X::do_lengthy_work, &my_x, 42); // 调用 my_x.do_lengthy_work(42)
	t.join();
}
```

- 提供的参数仅支持移动，不能拷贝（如 `std::unique_ptr` 类型参数）
- 使用移动操作可以将对象转换成函数可接受的实参类型，或满足函数返回值类型要求。
- 当变量时临时变量时，自动进行移动操作；当原对象是一个命名变量，转移的时候需要使用 `std::move` 进行显式移动。
- 见如下例子

```cpp
void process_big_object(std::unique_ptr<big_object> p);

void move_ownership_to_thread()
{
	std::unique_ptr<big_object> p(new big_object);
	p->prepare_data(42);
	// 通过在 `std::thread` 构造函数中执行 `std::move(p)`，
	// `big_object` 对象的所有权首先被转移到新创建的线程的内部存储中，之后再传递给 `proccess_big_object` 函数
	std::thread t(process_big_object, std::move(p));
	t.join();
}
```

- std::thread 的每个实例都负责管理一个线程（前提是传入了可调用对象）
- 线程的所有权可以在多个 `std::thread` 实例中转移（依赖于 `std::thread` 可移动、不可复制）
- 某个时间点，一个执行线程只被一个 `std::thread` 实例拥有

### 转移所有权

TODO



