#include <thread>
#include <iostream>
#include <chrono>

int main()
{
	std::thread t1([](){
		std::cout << "Hello from thread t1" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
	});

	t1.join();  // 主线程阻塞，等待t1对应的线程完成

	std::cout << "Hello from main thread" << std::endl; // 主线程继续执行

	return 0;
}
