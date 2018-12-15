#include <iostream>
#include <pthread.h>
#include <stdio.h>

using namespace std;

#define NUM_THREADS		5

int task = 10;
pthread_cond_t task_cond;
pthread_mutex_t sum_mutex;

void *say_hello1(void* arg)
{
	pthread_t pid = pthread_self();
	cout << "thread id [" <<  pid <<"] arg "<< *((int *)arg) << endl;
	
	while(1){
		pthread_mutex_lock(&sum_mutex);
		if(task > NUM_THREADS){
			cout << "pid ["<< pid <<"] take task " << task << " in thread " << *((int *)arg) << endl;
			pthread_cond_wait(&task_cond, &sum_mutex);
			cout << "pid take task " << *((int *)arg) << endl;
		}else{
			cout << "[*"<< pid <<"*] take task " << task << " in thread " << *((int *)arg)<< endl;
			--task;
		}
		pthread_mutex_unlock(&sum_mutex);
		
		if(task == 0)
			break;
	}		
}

void *say_hello2(void* arg)
{
	pthread_t pid = pthread_self();
	cout << "thread id [" <<  pid <<"] arg "<< *((int *)arg) << endl;
	
	bool is_signaled = false;
	
	while(1){
		pthread_mutex_lock(&sum_mutex);
		if(task > NUM_THREADS){
			cout << "pid [" << pid << "] take task "  << task << " in thread " << *((int *)arg) << endl;
			--task;
		}else if(!is_signaled){
			cout << "pid [*"<< pid <<"*] take task " << task << " in thread " << *((int *)arg) << endl;
			pthread_cond_signal(&task_cond);
			is_signaled = true;					
		}
		pthread_mutex_unlock(&sum_mutex);
		
		if(task == 0)
			break;
	}
}

int main()
{
	pthread_t tids[NUM_THREADS];
	int indexes[NUM_THREADS];
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&sum_mutex, NULL);
	pthread_cond_init(&task_cond,NULL);
	
	pthread_t tid1, tid2;		
		
	int ret = 0;
	
	int index1 = 1;
	ret = pthread_create(&tids[0], &attr, say_hello1, (void *)&index1);
	if(ret != 0){
		cout << "pthread_create error:error_code " << ret << endl;
	}

	int index2 = 2;
	ret = pthread_create(&tids[1], &attr, say_hello2, (void *)&index2);
	if(ret != 0){
		cout << "pthread_create error:error_code " << ret << endl;
	}	
		
	void *status = NULL;
	pthread_join(tids[0], &status);	
	pthread_join(tids[1], &status);
	
	pthread_attr_destroy(&attr);	
	pthread_mutex_destroy(&sum_mutex);
	pthread_cond_destroy(&task_cond);
}