#include <thread>
#include <iostream>

int main()
{
	std::thread t([](){
		std::cout << "Hello from thread" << std::endl;
	});
	t.join();  // 等待线程完成
	return 0;
}
