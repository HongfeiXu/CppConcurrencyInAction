#include <numeric>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>


// 函数对象：用于在单个线程中累加指定范围内的元素
template<typename Iterator, typename T>
struct accumulate_block
{
	// 函数调用运算符：累加 [first, last) 范围内的元素
	// first, last: 数据范围的迭代器
	// result: 引用参数，用于存储累加结果（避免拷贝开销）
	void operator()(Iterator first, Iterator last, T& result)
	{
		// 调用标准库的 accumulate 算法进行累加
		result = std::accumulate(first, last, result);
	}
};

// 并行累加函数：使用多线程并行计算 [first, last) 范围内元素的总和
// Iterator: 迭代器类型（支持任何容器）
// T: 累加结果的数据类型
// first, last: 定义要累加的数据范围 [first, last)
// init: 初始值（累加的起点）
// 返回值: 累加的总和
template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
	// 计算数据范围的长度（元素总数）
	unsigned long const length = std::distance(first, last);

	// 边界检查：如果范围为空，直接返回初始值，避免不必要的计算
	if(!length)
	{
		return init;
	}

	// 每个线程最小处理量：确保每个线程有足够的工作，避免过度并行导致的开销
	unsigned long const min_per_thread = 25;
	// 根据数据量和最小工作量计算最大线程数
	// 使用向上取整公式：(length + min_per_thread - 1) / min_per_thread
	unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

	// 获取硬件支持的并发线程数（可能返回0表示未知）
	unsigned long const hardware_threads = std::thread::hardware_concurrency();

	// 确定最终使用的线程数：
	// - 如果硬件信息可用，使用硬件线程数；否则保守地使用2个线程
	// - 取硬件线程数和数据量决定的线程数的最小值，避免过度并行
	unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

	// 计算每个线程平均处理的数据块大小（最后一个线程可能处理更多）
	unsigned long const block_size = length / num_threads;

	// 存储每个线程的累加结果
	std::vector<T> result(num_threads);
	// 存储工作线程（主线程不在这里，所以是 num_threads - 1）
	std::vector<std::thread> threads(num_threads - 1);

	// 初始化第一个数据块的起始位置
	Iterator block_start = first;

	// 为前 num_threads-1 个线程分配任务
	for(unsigned long i = 0; i < (num_threads - 1); ++i)
	{
		// 计算当前数据块的结束位置
		Iterator block_end = block_start;
		// 将迭代器向前移动 block_size 个位置
		std::advance(block_end, block_size);
		// 创建工作线程，执行累加操作
		// accumulate_block<Iterator, T>(): 创建函数对象
		// block_start, block_end: 数据范围
		// std::ref(result[i]): 通过引用传递结果，避免拷贝
		threads[i] = std::thread(
			accumulate_block<Iterator, T>(),
			block_start, block_end, std::ref(result[i])
		);
		// 移动到下一个数据块的起始位置
		block_start = block_end;
	}

	// 主线程处理最后一个数据块（可能包含剩余元素，因为 length / num_threads 可能有余数）
	accumulate_block<Iterator, T>()(
		block_start, last, result[num_threads - 1]
	);

	// 等待所有工作线程完成它们的任务
	for(auto&entry :threads)
	{
		entry.join();
	}

	// 将所有线程的结果累加起来，返回最终的总和
	return std::accumulate(result.begin(), result.end(), init);
}


void test()
{
	// 创建足够大的数据集以展示性能差异
	// 使用5000万个元素，确保有足够的工作量来展示并行优势
	const size_t data_size = 50000000;  // 5000万个元素
	std::vector<int> data(data_size);

	// 初始化数据：填充1到data_size的连续整数
	for(size_t i = 0; i < data_size; ++i)
	{
		data[i] = static_cast<int>(i + 1);
	}

	std::cout << "=== Parallel Accumulate Performance Test ===" << std::endl;
	std::cout << "Data size: " << data_size << " elements" << std::endl;
	std::cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << std::endl;

	// 计算实际使用的线程数（用于显示）
	unsigned long const length = std::distance(data.begin(), data.end());
	unsigned long const min_per_thread = 25;
	unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
	unsigned long const hardware_threads = std::thread::hardware_concurrency();
	unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
	std::cout << "Threads used: " << num_threads << std::endl;
	std::cout << std::endl;

	// 测试单线程版本
	std::cout << "--- Single-threaded std::accumulate ---" << std::endl;
	auto start_single = std::chrono::high_resolution_clock::now();
	long long result_single = std::accumulate(data.begin(), data.end(), 0LL);
	auto end_single = std::chrono::high_resolution_clock::now();
	auto duration_single_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		end_single - start_single
	).count();
	auto duration_single_us = std::chrono::duration_cast<std::chrono::microseconds>(
		end_single - start_single
	).count();

	std::cout << "Result: " << result_single << std::endl;
	std::cout << "Time: " << duration_single_ms << " ms (" << duration_single_us << " us)" << std::endl;
	std::cout << std::endl;

	// 测试并行版本
	std::cout << "--- Parallel parallel_accumulate ---" << std::endl;
	auto start_parallel = std::chrono::high_resolution_clock::now();
	long long result_parallel = parallel_accumulate(data.begin(), data.end(), 0LL);
	auto end_parallel = std::chrono::high_resolution_clock::now();
	auto duration_parallel_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		end_parallel - start_parallel
	).count();
	auto duration_parallel_us = std::chrono::duration_cast<std::chrono::microseconds>(
		end_parallel - start_parallel
	).count();

	std::cout << "Result: " << result_parallel << std::endl;
	std::cout << "Time: " << duration_parallel_ms << " ms (" << duration_parallel_us << " us)" << std::endl;
	std::cout << std::endl;

	// 验证结果正确性
	if(result_single == result_parallel)
	{
		std::cout << "[PASS] Result verification: Both methods produce the same result" << std::endl;
	}
	else
	{
		std::cout << "[FAIL] Error: Results do not match!" << std::endl;
		std::cout << "  Single-threaded result: " << result_single << std::endl;
		std::cout << "  Parallel result: " << result_parallel << std::endl;
	}

	// 性能对比（使用微秒精度）
	std::cout << std::endl;
	std::cout << "=== Performance Comparison ===" << std::endl;
	if(duration_parallel_us < duration_single_us)
	{
		double speedup = static_cast<double>(duration_single_us) / duration_parallel_us;
		std::cout << "Parallel version is faster! Speedup: " << speedup << "x" << std::endl;
		std::cout << "Performance improvement: " << ((duration_single_us - duration_parallel_us) * 100.0 / duration_single_us)
		          << "%" << std::endl;
	}
	else if(duration_parallel_us > duration_single_us)
	{
		std::cout << "Single-threaded version is faster (possibly due to thread overhead)" << std::endl;
		std::cout << "Performance degradation: " << ((duration_parallel_us - duration_single_us) * 100.0 / duration_single_us)
		          << "%" << std::endl;
	}
	else
	{
		std::cout << "Performance is similar" << std::endl;
	}
}
