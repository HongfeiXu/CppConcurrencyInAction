#include <thread>
#include <vector>
#include <iostream>

void do_work(unsigned id)
{
	std::cout << "Thread " << id << " is working." << std::endl;
}

void f()
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

void test()
{
	f();
}
