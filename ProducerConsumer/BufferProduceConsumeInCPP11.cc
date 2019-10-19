#include <iostream>
#include <vector>
#include <condition_variable>
#include <thread>

using namespace std;

class BoundedBuffer {
public:
	BoundedBuffer(const BoundedBuffer& rhs) = delete;
	BoundedBuffer& operator=(const BoundedBuffer& rhs) = delete;

	BoundedBuffer(std::size_t size) :
		begin_(0), end_(0), buffered_(0), circul_buffer_(size) {}

	void produece(int n)
	{
		{
			unique_lock<mutex> lock(mutex_);
			not_full_cv.wait(lock,
				[=] {return buffered_ < circul_buffer_.size(); });
			circul_buffer_[end_] = n;
			end_ = (end_ + 1) % circul_buffer_.size();
			++buffered_;
		}


		not_empty_cv.notify_one();
	}

	int consume()
	{
		unique_lock<mutex> lock(mutex_);
		not_empty_cv.wait(lock,
			[=] {return buffered_ > 0; });
		int n = circul_buffer_[begin_];
		begin_ = (begin_ + 1) % circul_buffer_.size();
		--buffered_;

		lock.unlock();

		not_full_cv.notify_one();
		return n;
	}

private:
	size_t begin_;
	size_t end_;
	size_t buffered_;
	vector<int> circul_buffer_;

	condition_variable not_full_cv;
	condition_variable not_empty_cv;
	mutex mutex_;
};

BoundedBuffer g_buffer(2);
mutex g_io_mutex;

void Producer()
{
	int n = 0;
	while (n < 100000)
	{
		g_buffer.produece(n);
		if (0 == (n % 10000))
		{
			unique_lock<mutex> lock(g_io_mutex);
			cout << "Produce " << n << endl;
		}
		n++;
	}
	g_buffer.produece(-1);
}

void Consumer()
{
	thread::id thread_id = this_thread::get_id();

	int n = 0;
	do {
		n = g_buffer.consume();
		if (0 == (n % 10000))
		{
			unique_lock<mutex> lock(g_io_mutex);
			cout << "thread_id " << thread_id << "Consumer " << n << endl;
		}
	} while (n != -1);

	g_buffer.produece(-1);
}

int main()
{
	std::cout << "Hello World!\n";

	vector<thread> threads;

	threads.push_back(thread(&Producer));
	threads.push_back(thread(&Consumer));
	//threads.push_back(thread(&Consumer));
	//threads.push_back(thread(&Consumer));

	for (auto& t : threads)
		t.join();

	return 0;
}