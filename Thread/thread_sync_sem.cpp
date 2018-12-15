#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem1;
sem_t sem2;

void myThread1(void)
{
	sem_wait(&sem1);
	for(int i = 0; i < 5;i++ )
	{
		printf("[1]Hello thrad-%d\n", i);
		sleep(1);
	}
	sem_post(&sem2);
	
}

void myThread2(void)
{
	sem_wait(&sem2);
	for(int i= 0; i < 5;i++ )
	{
		printf("[2]Hello thrad-%d\n",i);
		sleep(1);
	}
	sem_post(&sem1);
	
}

int main()
{
	int ret = 0;
	pthread_t id1;
	pthread_t id2;
	
	sem_init(&sem1, 0, 1);
	sem_init(&sem2, 0, 1);
	
	ret = pthread_create(&id1, NULL, (void*(*)(void*))myThread1, NULL);
	if(ret)
	{
		printf("ceeate pthread error");
		return 1;
	}
	
	
	ret = pthread_create(&id2, NULL, (void*(*)(void*))myThread2, NULL);
	if(ret)
	{
		printf("ceeate pthread error");
		return 1;
	}
	
	
	pthread_join(id1, NULL);
	pthread_join(id2, NULL);
	

	sem_destroy(&sem1);
	sem_destroy(&sem2);
	
	return 0;
}
