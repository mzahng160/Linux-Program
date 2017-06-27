#include <iostream>
#include <pthread.h>

using namespace std;

#define NUM_THREADS		5

int sum = 0;
pthread_mutex_t sum_mutex;

void *say_hello(void* args)
{
	
	pthread_mutex_lock(&sum_mutex);
	cout << "deal id " << *((int *)args) ;
	cout << " before " <<sum; 
	sum += *((int *)args);
	cout << "after" << sum << endl;
	
	pthread_mutex_unlock(&sum_mutex);
	
	pthread_exit(0);
}

int main()
{
	pthread_t tids[NUM_THREADS];
	int indexes[NUM_THREADS];
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&sum_mutex, NULL);
	
	int ret = 0;
	
	for(int i = 0; i < NUM_THREADS; ++i)
	{
		indexes[i] = i;
		
		ret = pthread_create(&tids[i], &attr, say_hello, (void *)&i);
		if(ret != 0){
			cout << "pthread_create error:error_code " << ret << endl;
		}				
	}
	
	pthread_attr_destroy(&attr);
		
	void *status;
	for(int i = 0; i < NUM_THREADS; ++i){
		ret = pthread_join(tids[i], &status);
		
		if(ret != 0){
			cout << "pthread_join error:error_code = " << ret << endl;
		}		
	}
	cout << "finally sum is" << sum <<endl;
	
	pthread_mutex_destroy(&sum_mutex);
}