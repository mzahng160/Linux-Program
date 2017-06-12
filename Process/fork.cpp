#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

void print_exit(){
	printf("the exit pid : %d\n", getpid());
}

int main()
{
	int i = 0;
	if(fork() == 0){	//child
		for(i = 1; i < 10; i++)
			printf("child : %d\n", i);
	}else{				//parent
		for(i = 0; i < 10; i++)
			printf("parent : %d\n", i);
	}
}