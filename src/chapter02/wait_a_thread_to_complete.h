#include <thread>
#include <iostream>


struct func
{
	int& i;
	func(int& i_) : i(i_) {}

	void operator()()
	{
		for(unsigned j = 0; j < 100000000; ++j)
		{
			++i; // Potential access to dangling reference (悬空引用)
		}
	}
};

inline void do_something_in_current_thread()
{
	std::cout << "do_something_in_current_thread" << std::endl;
}


// Listing 2.1 Function already returned, thread still accesses local variable
// 使用 detach() 方式启动线程，线程执行过程中抛出异常导致无法正确退出
inline void oops()
{
	int some_local_state = 0; // 局部变量
	func my_func(some_local_state);
	std::thread my_thread(my_func);
	my_thread.detach();

} // 函数返回，some_local_state 被销毁，但线程可能还在运行！
// 修复方式：使用 join() 等待线程完成，或确保线程不依赖已销毁的局部变量


// Listing 2.2 Waiting for a thread to finish
// use try/catch block 目的是保证 t.join() 能被调用到，避免线程执行过程中抛出异常导致无法正确退出
inline void f_1()
{
	int some_local_state = 0;
	func my_func(some_local_state);
	std::thread t(my_func);
	try{
		do_something_in_current_thread();
	}
	catch(...)
	{
		t.join(); // exits by an exception
		throw;
	}
	t.join(); // exits normally
}


// Listing 2.3 Using RAII to wait for a thread to complete
// 相比 Listing 2.2，使用 RAII 方式等待线程完成，避免线程执行过程中抛出异常导致无法正确退出，同时避免了 try/catch 块的嵌套
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


inline void f_2()
{
	int some_local_state = 0;
	func my_func(some_local_state);
	std::thread t(my_func);
	thread_guard g(t);

	do_something_in_current_thread();
}

inline void test()
{
	// oops();
	f_1();
	f_2();
}
