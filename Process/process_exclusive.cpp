#include    <stdio.h>  
#include    <stdlib.h>  
#include    <unistd.h>  
#include    <fcntl.h>  
#include    <sys/mman.h>  
#include    <pthread.h>  
pthread_mutex_t* g_mutex;  
//创建共享的mutex  
void init_mutex(void)  
{  
    int ret;  
    //g_mutex一定要是进程间可以共享的，否则无法达到进程间互斥  
    g_mutex=(pthread_mutex_t*)mmap(NULL, sizeof(pthread_mutex_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);  
    if( MAP_FAILED==g_mutex )  //使用匿名存储映射，只能在具有亲缘关系的进程间使用，
    {                                      //若要在非亲缘关系进程间使用需要mmap文件映射结合SHARED                                             //标志或者共享内存IPC
        perror("mmap");  
        exit(1);  
    }  
      
    //设置attr的属性  
    pthread_mutexattr_t attr;  
    pthread_mutexattr_init(&attr);  
    //一定要设置为PTHREAD_PROCESS_SHARED  
    //具体可以参考http://blog.chinaunix.net/u/22935/showart_340408.html  
    ret=pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);  
    if( ret!=0 )  
    {  
        perror("init_mutex pthread_mutexattr_setpshared");  
        exit(1);  
    }  
    pthread_mutex_init(g_mutex, &attr);  
}  
int main(int argc, char *argv[])  
{  
    init_mutex();  
    int ret;      
    char str1[]="this is child process/r/n";  
    char str2[]="this is father process/r/n";  
    int fd=open("tmp", O_RDWR|O_CREAT|O_TRUNC, 0666);  
    if( -1==fd )  
    {  
        perror("open");  
        exit(1);  
    }  
    pid_t pid;  
    pid=fork();  
    if( pid<0 )  
    {  
        perror("fork");  
        exit(1);  
    }  
    else if( 0==pid )  
    {  
        ret=pthread_mutex_lock(g_mutex);  
        if( ret!=0 )  
        {  
            perror("child pthread_mutex_lock");  
        }  
        sleep(10);//测试是否能够阻止父进程的写入  
        write(fd, str1, sizeof(str1));  
        ret=pthread_mutex_unlock(g_mutex);    
        if( ret!=0 )  
        {  
            perror("child pthread_mutex_unlock");  
        }     
    }  
    else  
    {  
        sleep(2);//保证子进程先执行   
        ret=pthread_mutex_lock(g_mutex);  
        if( ret!=0 )  
        {  
            perror("father pthread_mutex_lock");  
        }  
        write(fd, str2, sizeof(str2));  
        ret=pthread_mutex_unlock(g_mutex);    
        if( ret!=0 )  
        {  
            perror("father pthread_mutex_unlock");  
        }                 
    }  
    wait(NULL);  
    munmap(g_mutex, sizeof(pthread_mutex_t));  
} 