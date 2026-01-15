#include <string>
#include <thread>
#include <cstdio>
#include <iostream>

// 例1：动态变量的指针作为参数传递给线程，可能导致悬空指针问题
/*
std::thread 构造函数会拷贝参数，但 C 风格字符串（char*）会被当作指针拷贝，而不是内容
使用 std::string(buffer) 显式构造一个字符串对象，确保拷贝的是字符串内容
由于 detach()，线程在后台运行，必须确保所有参数都是独立的副本，不依赖原作用域
*/

void f(int i, std::string const& s)
{
	std::cout << "f(" << i << ", " << s << ")" << std::endl;
}

void oops(int some_param)
{
	char buffer[1024]; // 1
	sprintf(buffer, "%i", some_param);
	std::thread t(f, 3, buffer); // 2
	t.detach();
}

void not_oops(int some_param)
{
	char buffer[1024];
	sprintf(buffer, "%i", some_param);
	std::thread t(f, 3, std::string(buffer));  // 使用std::string，避免悬空指针
	t.detach();
}

// 例2：std::thread 构造函数无视函数参数类型，盲目地拷贝已提供的变量
// 解决方案：使用 std::ref 将参数转换成引用的形式

class widget_id
{
public:
	int id;
	widget_id(int id_) : id(id_) {}
};


class widget_data
{
public:
	void update(widget_id w)
	{
		std::cout << "update(" << w.id << ")" << std::endl;
	}
};


void update_data_for_widget(widget_id w, widget_data& data)
{
	data.update(w);
}


void pass_reference_to_thread(widget_id w)
{
	widget_data data;
	// data 会被拷贝，以右值的方式传递给 update_data_for_widget，编译出错
	// std::thread t(update_data_for_widget, w, data);

	// data 会被以引用的形式传递给 update_data_for_widget，编译通过
	std::thread t(update_data_for_widget, w, std::ref(data));
	t.join();
}


// 例3：调用对象的成员函数作为线程函数

class X
{
public:
	void do_lengthy_work(int i)
	{
		std::cout << "do_lengthy_work(" << i << ")" << std::endl;
	}
};

void call_member_function_as_thread_function()
{
	X my_x;
	std::thread t(&X::do_lengthy_work, &my_x, 42); // 调用 my_x.do_lengthy_work(42)
	t.join();
}


void test()
{
	// oops(1);
	not_oops(2);
	pass_reference_to_thread(widget_id(3));
	call_member_function_as_thread_function();
}
