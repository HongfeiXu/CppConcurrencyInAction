#include <thread>
#include <vector>
#include <iostream>
#include <mutex>


inline std::mutex& cout_mutex()
{
	static std::mutex m;
	return m;
}


inline void do_work(unsigned id)
{
	// std::cout is not thread-safe, use a mutex to protect it
	std::lock_guard<std::mutex> lk(cout_mutex());
	std::cout << "Thread " << id << " is working." << std::endl;
}

inline void f()
{
	std::vector<std::thread> threads;

	for (unsigned i = 0; i < 5; ++i)
	{
		threads.emplace_back(do_work, i);
	}

	for (auto& t : threads)
	{
		if (t.joinable()) // 此检查非必须，但是个好习惯 ;)
		{
			t.join();
		}
	}
}

inline void test()
{
	f();
}
