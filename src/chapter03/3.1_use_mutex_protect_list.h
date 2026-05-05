#pragma once

#include <list>
#include <mutex>
#include <algorithm>
#include <thread>
#include <iostream>

namespace chapter03::section_3_1
{

inline std::list<int> some_list;
inline std::mutex some_mutex;

inline void add_to_list(int new_value)
{
	std::lock_guard<std::mutex> guard(some_mutex);
	some_list.push_back(new_value);
}

inline bool list_contains(int value_to_find)
{
	std::lock_guard<std::mutex> guard(some_mutex);
	return std::find(some_list.begin(), some_list.end(), value_to_find) != some_list.end();
}

inline void test_3_1_use_mutex_protect_list()
{
	std::cout << "--- test_3_1_use_mutex_protect_list ---" << std::endl;

	{
		std::lock_guard<std::mutex> guard(some_mutex);
		some_list.clear();
	}

	std::thread t1([]{
		for(int i = 0; i < 1000; ++i)
		{
			add_to_list(i);
		}
	});

	std::thread t2([]{
		for(int i = 1000; i < 2000; ++i)
		{
			add_to_list(i);
		}
	});

	t1.join();
	t2.join();

	std::cout << "contains 1420 = " << list_contains(1420) << std::endl;
	std::cout << "contains 3000 = " << list_contains(3000) << std::endl;

	{
		std::lock_guard<std::mutex> guard(some_mutex);
		std::cout << "list size = " << some_list.size() << std::endl;
	}
}

}
