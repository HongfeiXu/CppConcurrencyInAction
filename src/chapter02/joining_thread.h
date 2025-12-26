#include <thread>
#include <iostream>
#include <chrono>

class joining_thread
{
	std::thread t;
public:
	joining_thread() noexcept=default;

	// 作用：接受一个可调用对象（函数、lambda等）和任意参数，创建并启动线程
	// - Callable&& func：万能引用，可以接受任何可调用对象
	// - Args&& ... args：可变参数模板，接受任意数量的参数
	// - std::forward: 完美转发，保持参数的原始类型（左值/右值）
	template<typename Callable, typename ... Args>
	explicit joining_thread(Callable&& func, Args&& ... args):
		t(std::forward<Callable>(func), std::forward<Args>(args)...) {}

	// 从 std::thread 构造
	// 接管一个已存在的 std::thread 对象的所有权
	explicit joining_thread(std::thread t_): t(std::move(t_)) {}

	// 移动构造函数
	// 从另一个 joining_thread 转移线程所有权。移动后，other 不再关联任何线程
	joining_thread(joining_thread&& other) noexcept: t(std::move(other.t)) {}

	// 移动赋值运算符
	// 关键点：赋值前会先 join() 当前线程！这确保不会丢弃一个正在运行的线程
	joining_thread& operator=(joining_thread&& other) noexcept
	{
		if(joinable()) join(); 	// 先等待当前线程完成
		t = std::move(other.t); // 然后接管新线程
		return *this;
	}

	// 从 std::thread 赋值
	// 与移动赋值运算符同样的逻辑，接管一个 std::thread。
	joining_thread& operator=(std::thread other) noexcept
	{
		if(joinable()) join();
		t = std::move(other);
		return *this;
	}

	// 析构函数
	// 普通 std::thread 如果在析构时仍然 joinable()（即线程还在运行且未被 join/detach），程序会调用 std::terminate() 崩溃
	// joining_thread 在析构时自动 join()，确保线程安全结束
	~joining_thread() noexcept
	{
		if(joinable()) join();
	}

	// 禁止拷贝构造
	// 线程不能被拷贝（一个线程只能有一个所有者），只能移动。
	joining_thread(joining_thread const&)=delete;

	// 禁止拷贝赋值运算符
	// 线程不能被拷贝（一个线程只能有一个所有者），只能移动。
	joining_thread& operator=(joining_thread const&)=delete;

	void swap(joining_thread& other) noexcept
	{
		t.swap(other.t);
	}

	std::thread::id get_id() const noexcept
	{
		return t.get_id();
	}

	bool joinable() const noexcept
	{
		return t.joinable();
	}

	void join()
	{
		t.join();
	}

	void detach()
	{
		t.detach();
	}

	std::thread& as_thread() noexcept
	{
		return t;
	}

	const std::thread& as_thread() const noexcept
	{
		return t;
	}
};


inline void test()
{
	joining_thread j1([]{
		std::cout << "Thread 1 start\n";
		std::this_thread::sleep_for(std::chrono::seconds(2));
		std::cout << "Thread 1 end\n";
	});

	joining_thread j2([]{
		std::cout << "Thread 2 start\n";
	});

	std::cout << "j1.get_id() = " << j1.get_id() << std::endl;
	std::cout << "j2.get_id() = " << j2.get_id() << std::endl;
	j1 = std::move(j2); // 调用 operator= 函数，将 j2 对应的线程的所有权转移给 j1，其中包含了先等待当前线程完成，然后接管新线程的操作
	std::cout << "j1.get_id() = " << j1.get_id() << std::endl;
	std::cout << "j2.get_id() = " << j2.get_id() << std::endl; // 0 表示没有线程

	std::cout << "Assignment done\n";
}

