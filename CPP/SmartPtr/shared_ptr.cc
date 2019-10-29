#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>

struct Base
{
	Base() { std::cout << " Base::Base()\n"; }
	~Base() { std::cout << " Base::~Base()\n"; }
};

struct Derived : public Base
{
	Derived() { std::cout << " Derived::Derived()\n"; }
	~Derived() { std::cout << " Derived::~Derived()\n"; }
};

struct C { int* data; };

class A
{	
public:
	A(int n) :i(n) { }
	~A() { std::cout << " ~A():destructed " << i << std::endl; }

	int i;

};

void thrfunc(std::shared_ptr<Base> p)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::shared_ptr<Base> lp = p;
	{
		static std::mutex io_mutex;
		std::lock_guard<std::mutex> lk(io_mutex);
		std::cout << "local pointer in a thread:\n"
			<< " lp.get() = " << lp.get()
			<< ", lp.use_count() = " << lp.use_count() << "\n";
	}
}

void thrfunc_2(std::shared_ptr<A> lp)
{
	//std::this_thread::sleep_for(std::chrono::seconds(1));

	std::shared_ptr<A> p = lp;	
	{
		//static std::mutex io_mutex;
		//std::lock_guard<std::mutex> lk(io_mutex);
		for (int i = 0; i < 5; i++)
		{
			int x = p->i;
			x++;
			p->i = x;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		//std::cout << "p get " << p.get() << " data " << p->i << " count " << p.use_count() << "\n";
		printf("p get():%p, data:%d, count:%ld\n", p.get(), p->i, p.use_count());
	}
	
}

int main()
{
#ifdef FIRSTSECTION
	std::shared_ptr<Base> p = std::make_shared<Derived>();
	std::cout << "Created a shared Derived (as a pointer to base)\n"
		<< " p.get()" << p.get()
		<< ", p.use_count = " << p.use_count() << "\n";
	std::thread t1(thrfunc, p), t2(thrfunc, p), t3(thrfunc, p);
	p.reset();
	std::cout << "shared ownership between 3 threads and released\n"
		<< "ownership from main"
		<< " p.get()" << p.get()
		<< ", p.use_count = " << p.use_count() << "\n";

	t1.join(); t2.join(); t3.join();
	std::cout << "All threads completed, the last one deleted Derived\n";
#endif

#ifdef SECONDSECTION
	std::shared_ptr<C> obj(new C);
	std::cout << "obj get:" << obj.get()  
		<< " count:" << obj.use_count() << "\n";

	std::shared_ptr<int> p9(obj, obj->data);
	std::cout << "obj get:" << p9.get() 
		<< " count:" << p9.use_count() << "\n";
#endif	
	
	std::shared_ptr<A> sp1(new A(2));
	std::cout << "sp1 get:" << sp1.get() << " data " << sp1->i << " count " << sp1.use_count() << "\n";

	std::shared_ptr<A> sp2(sp1);
	std::cout << "sp2 get " << sp2.get() << " data " << sp2->i << " count " << sp2.use_count() << "\n";

	std::shared_ptr<A> sp3;
	sp3 = sp2;	
	std::cout << "sp3 get " << sp3.get() << " data " << sp3->i << " count " << sp3.use_count() << "\n";

	std::thread t1(thrfunc_2, sp1), t2(thrfunc_2, sp2), t3(thrfunc_2, sp3);

	std::cout << std::endl;

#if 0
	sp1.reset(new A(3));
	sp2.reset(new A(4));
	sp3.reset(new A(5));

	std::cout << "sp1 get:" << sp1.get() << " data " << sp1->i << " count " << sp1.use_count() << "\n";
	std::cout << "sp2 get " << sp2.get() << " data " << sp2->i << " count " << sp2.use_count() << "\n";
	std::cout << "sp3 get " << sp3.get() << " data " << sp3->i << " count " << sp3.use_count() << "\n";
#endif

	t1.join(); t2.join(); t3.join();
}