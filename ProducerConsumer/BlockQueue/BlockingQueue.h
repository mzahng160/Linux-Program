#pragma once
#include <deque>
#include <condition_variable>
#include <mutex>

template<typename T>
class BlockingQueue
{
public:
	BlockingQueue()
	{}
	//BlockingQueue operator=() = delete;
	//BlockingQueue(BlockingQueue&) = delete;

	void put(const T& x)
	{		
		std::lock_guard <std::mutex> lck(mutex_);
		queue_.push_back(x);
		notEmpty_.notify_one(); // wait morphing saves us							
	}

	void put(T&& x)
	{
		std::lock_guard <std::mutex> lck(mutex_);
		queue_.push_back(std::move(x));
		notEmpty_.notify_one();
	}

	T take()
	{
		std::unique_lock<std::mutex> lck(mutex_);
		// always use a while-loop, due to spurious wakeup
		while (queue_.empty()) 
			notEmpty_.wait(lck);
		//assert(!queue_.empty());
		T front(std::move(queue_.front()));
		queue_.pop_front();
		return std::move(front);
	}

private:
	mutable std::mutex mutex_;
	std::condition_variable         notEmpty_ ;
	std::deque<T>     queue_;
};