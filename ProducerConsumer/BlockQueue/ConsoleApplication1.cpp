// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include "BlockingQueue.h"

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

BlockingQueue<std::string> queue;

void produce()
{
}

void consumer()
{
	bool running = true;

	while (running)
	{
		std::string d(queue.take());
		//printf("get data = %s, size = %zd\n", d.c_str());
		running = (d != "stop");
	}
}

void go() 
{
	std::unique_lock<std::mutex> lck(mtx);
	ready = true;
	cv.notify_all();
}

int main()
{
	std::thread thds[10];

	for (int i =0; i < 5; ++i)
		thds[i] = std::thread(produce);

	for (int i = 0; i < 5; ++i)
		thds[i] = std::thread(consumer);

	go();

	for (auto& th : thds) th.join();

	getchar();
    return 0;
}

