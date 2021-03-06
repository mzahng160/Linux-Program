// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include "BlockingQueue.h"

std::mutex mtx;
std::condition_variable cv;
bool ready = false;
bool stoppush = false;

BlockingQueue<std::string> queue;
std::atomic_int count = 100000;

void produce()
{
	while (!stoppush)
	{
		char buf[32];
		int tmp = --count;
		if (tmp <= 0)
			snprintf(buf, sizeof buf, "stop");
		else
			snprintf(buf, sizeof buf, "[%d]testhello [%d]", tmp, tmp);

		queue.put(buf);
	}
}

void consumer()
{
	bool running = true;

	while (running)
	{
		std::string d(queue.take());
		printf("get data = %s\n", d.c_str());
		running = (0 != d.compare("stop"));
	}

	stoppush = true;
}

void go() 
{
	std::unique_lock<std::mutex> lck(mtx);
	ready = true;
	cv.notify_all();
}

int main()
{
	const int n = 2;
	std::thread thds[n];

	int i = 0;
	for (; i < 1; ++i)
		thds[i] = std::thread(consumer);

	for (; i < n; ++i)
		thds[i] = std::thread(produce);



	//go();

	for (auto& th : thds) th.join();

	getchar();
    return 0;
}

