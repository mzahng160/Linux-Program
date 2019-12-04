#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/shm.h>
#include <sys/wait.h>
/* 
IPC_PRIVATE key值属性为0，和IPC对象的编号就没有了对应关系，和无名管道类似，
不能用于毫无关系的进程间通信，可以用于有亲缘关系的进程间通信
父进程与子进程间的通信,子进程写信息到共享内存中,父进程读取该信息
BUFSIZ为全局量定义在<stdlib.h>,8192 
使用exit(0)和wait(0)进行同步*/
int main()
{
    int chld,shmid;
    char *viraddr;
    char buffer[BUFSIZ];
    
    shmid=shmget(IPC_PRIVATE,BUFSIZ,0666|IPC_CREAT);
    viraddr=(char*)shmat(shmid,0,0);
    
    while((chld=fork())==-1);
    if(chld==0)
    {                                   /*子进程块*/
        while(1)
        {
            puts("Enter some text:");   /*写信息到共享内存*/
            fgets(buffer,BUFSIZ,stdin); /*用户输入信息*/
            strcat(viraddr,buffer);     /*附接到进程的虚拟空间*/
            if(strncmp(buffer,"end",3)==0)
                break;                  /*输入end结束*/
        }
        exit(0);
    }
    else
    {                                   /*父进程块*/
        wait(0);
        printf("Your message is:\n%s",viraddr);
        
        shmdt(viraddr);                 /*断开共享内存*/
        shmctl(shmid,IPC_RMID,0);       /*释放共享内存*/
        exit(0);
    }
}
