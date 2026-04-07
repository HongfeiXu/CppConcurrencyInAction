#include <vector>
#include <thread>
#include <iostream>

#include <mutex>
#include <chrono>

inline void test_3_2_mutex()
{
	std::cout << "--- test_3_2_mutex ---" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	const size_t data_size = 5e7;
	std::vector<int> data(data_size);
	for(size_t i = 0; i < data_size; ++i)
	{
		data[i] = static_cast<int>(i + 1);
	}

	long long sum = 0;
	std::mutex mtx;
	std::thread t1([&] {
		for(int i = 0; i < 1e7; ++i)
		{
			std::lock_guard<std::mutex> lock(mtx); // 构造时加锁，析构时自动解锁
			sum += data[i]; // 临界区：只有持有锁的线程能执行
		}
	});
	std::thread t2([&] {
		for(int i = 1e7; i < 2e7; ++i)
		{
			std::lock_guard<std::mutex> lock(mtx);
			sum += data[i];
		}
	});
	t1.join();
	t2.join();
	std::cout << "sum = " << sum << std::endl;

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "duration = " << duration.count() << "ms" << std::endl;
}
