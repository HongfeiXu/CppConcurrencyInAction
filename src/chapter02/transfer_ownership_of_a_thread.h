#include <thread>
#include <iostream>


class func
{
public:
	int i;
	func(int i_) : i(i_) {}
	void operator()()
	{
		for(unsigned j = 0; j < 100000000; ++j)
		{
			++i;
		}
	}
};

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

inline void do_something_in_current_thread()
{
	std::cout << "do_something_in_current_thread" << std::endl;
}

inline void f()
{
	int some_local_state = 0;
	scoped_thread t{std::thread(func(some_local_state))};
	do_something_in_current_thread();

	//scoped_thread t2{ std::thread() }; // throws logic_error
}

inline void test()
{
	f();
}
