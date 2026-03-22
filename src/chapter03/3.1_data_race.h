#include <vector>
#include <thread>
#include <iostream>

#include <mutex>
#include <chrono>

void test_3_1_data_race()
{
	std::cout << "--- test_3_1_data_race ---" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	const size_t data_size = 5e7;
	std::vector<int> data(data_size);
	for(size_t i = 0; i < data_size; ++i)
	{
		data[i] = static_cast<int>(i + 1);
	}

	long long sum = 0;
	// sum += ... 不是原子操作，而是“读-改-写”三步，两个线程可能交错执行，导致结果错误。
	std::thread t1([&] {for(int i = 0; i < 1e7; ++i) sum += data[i];});
	std::thread t2([&] {for(int i = 1e7; i < 2e7; ++i) sum += data[i];});
	t1.join();
	t2.join();
	std::cout << "sum = " << sum << std::endl;

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "duration = " << duration.count() << "ms" << std::endl;
}
