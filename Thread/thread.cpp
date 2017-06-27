#include <iostream>
#include <stdio.h>
#include <pthread.h>

using namespace std;

#define NUM_THREADS	5

void* say_hello(void *arg)
{
	char* msg = ((char *)arg);
	
	for(int i = 0; i < NUM_THREADS; ++i)
	{
		cout << msg;
		fflush(stdout);
		//sleep(1);
	}
}

int main()
{
	pthread_t tids[NUM_THREADS];
	
	int ret1 = pthread_create(&tids[0], NULL, say_hello, (void *)"hello");
	int ret2 = pthread_create(&tids[1], NULL, say_hello, (void *)"word\n");
	
	//int ret = 0;
	//say_hello((void *)&i);
	
	if(ret1 != 0)
	{
		cout << "pthread_create error:error_code = " << ret1 <<endl;
	}
	
	if(ret2 != 0)
	{
		cout << "pthread_create error:error_code = " << ret2 <<endl;
	}
	
	pthread_join(tids[0], NULL);
	pthread_join(tids[1], NULL);
}