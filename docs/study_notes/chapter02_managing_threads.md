# Chapter 2: Managing threads

## 主要内容

- 启动新线程
- 等待与分离
- 唯一标识符

## 2.1 线程的基本操作

等待线程完成，join 是阻塞操作

```cpp
#include <thread>
#include <iostream>
#include <chrono>

int main()
{
	std::thread t1([](){
		std::cout << "Hello from thread t1" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
	});

	t1.join();  // 主线程阻塞，等待t1对应的线程完成

	std::cout << "Hello from main thread" << std::endl; // 主线程继续执行

	return 0;
}
```

Using RAII to wait for a thread to complete

```cpp
// wait_a_thread_to_complete.h
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

## 2.2 传递参数

### 传参的基本形式

```cpp
void f(int i, std::string const& s);
std::thread t(f, 3, "hello");
```

---


### 参数默认通过值传递而非引用

- ==参数通过值传递：std::thread 构造函数会移动或拷贝参数，不会直接按引用传递。==
- 需要引用时需包装：要用引用传递时，用 std::ref（可修改）或 std::cref（只读）包装
- 见如下例子：

```cpp
void update_data_for_widget(widget_id w, widget_data& data);
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

引申问题：C++如此设计的意义是啥？

答：默认拷贝/移动是 安全默认（避免无意悬空引用、适配异步启动）；std::ref/std::cref 是 显式选择共享别名，把“我要保证生命周期”的责任写清楚。例子里 data 不能直接当 widget_data& 传进去，正是因为默认会 decay/按值处理，不会偷偷绑定到栈上的引用。

---

### 成员函数作为线程入口

- 使用成员函数指针与对象指针；`std::thread` 从第 3 个参数起对应成员函数的第 1 个实参，以此类推
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

---

### 仅可移动实参与 `std::move`

- 提供的参数仅支持移动、不能拷贝（如 `std::unique_ptr`）
- 使用移动可将对象转换成线程函数可接受的实参类型（或满足返回值等要求）
- 实参为临时量时常隐式移动；实参为命名变量时，传入线程存储通常需 `std::move` 显式移动
- 见如下例子

```cpp
void process_big_object(std::unique_ptr<big_object> p);

void move_ownership_to_thread()
{
	std::unique_ptr<big_object> p(new big_object);
	p->prepare_data(42);
	// 通过在 `std::thread` 构造函数中执行 `std::move(p)`，
	// `big_object` 对象的所有权首先被转移到新创建的线程的内部存储中，之后再传递给 `process_big_object` 函数
	std::thread t(process_big_object, std::move(p));
	t.join();
}
```

---

### 线程句柄的唯一所有权

- 在 `joinable()` 为真时，每个 `std::thread` 对象至多对应一个执行线程；默认构造或已 `join`/`detach` 后不关联线程
- 线程所有权可在多个 `std::thread` 对象间转移（`std::thread` 可移动、不可复制）
- 任一时刻，一个已启动的执行线程只被一个 `std::thread` 实例拥有（与下一节「2.3 转移所有权」衔接）

## 2.3 转移所有权

`std::thread` 可移动，但不可复制，类似的还有 `std::ifstream`、`std::unique_ptr` 等。


例子. 在t1、t2、t3之间转移所有权

```cpp
void some_function();
void some_other_function();
std::thread t1(some_function);
std::thread t2 = std::move(t1); // 将t1对应的线程的所有权转移给t2，因为t1是一个命名对象，需要显式调用 std::move
t1 = std::thread(some_other_function); // 临时 std::thread 对象对应的线程启动，所有权转移给t1
std::thread t3; // 创建一个空的 std::thread 对象，不关联任何线程
t3 = std::move(t2); // 将 t2 对应的线程的所有权转移给 t3
t1 = std::move(t3); // 将 t3 对应的线程的所有权转移给 t1，注意：这里会调用 std::terminate() 终止程序，因为 t1 已经关联了一个线程
```

例子 得到所有权，并负责线程的汇入

```cpp
// scoped_thread
// transfer_ownership_of_a_thread.h
class scoped_thread
{
	std::thread t;
public:
	explicit scoped_thread(std::thread t_): t(std::move(t_))
	{
		if(!t.joinable())
		{
			throw std::logic_error("No thread");
		}
	}
	~scoped_thread()
	{
		t.join();
	}
};
```

为什么这里需要 `explicit` ？
1. 防止意外转换：避免无意中将 `std::thread` 隐式转换成 `scoped_thread`
2. 明确意图：强制使用者显式创建 `scoped_thread`，说明这是一个重要的资源管理操作
3. RAII 原则：确保对象创建的明确性和可追踪性

例子 joining_thread 的实现

和C++20 中引入了 std::jthread 一个东西，
见 joining_thread.h


例子 量产线程，等待他们结束
见 batch_create_threads.h


## 2.4 确定线程数量

例子 并行版 std::accumulate
见 `2.4_multi_thread_accumulate.h`

