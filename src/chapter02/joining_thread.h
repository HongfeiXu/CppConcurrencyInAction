#include <thread>
#include <iostream>
#include <chrono>

class joining_thread
{
	std::thread t;
public:
	joining_thread() noexcept=default;

	template<typename Callable, typename ... Args>
	explicit joining_thread(Callable&& func, Args&& ... args):
		t(std::forward<Callable>(func), std::forward<Args>(args)...) {}

	explicit joining_thread(std::thread t_): t(std::move(t_)) {}

	joining_thread(joining_thread&& other) noexcept: t(std::move(other.t)) {}

	joining_thread& operator=(joining_thread&& other) noexcept
	{
		if(joinable()) join(); 	// 先等待当前线程完成
		t = std::move(other.t); // 然后接管新线程
		return *this;
	}

	joining_thread& operator=(std::thread other) noexcept
	{
		if(joinable()) join();
		t = std::move(other);
		return *this;
	}

	~joining_thread() noexcept
	{
		if(joinable()) join();
	}

	joining_thread(joining_thread const&)=delete;

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

