#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
 
#define BUF_SIZE 100
 
int main(int argc, char **argv)
{
    int fd, nread, i;
    struct stat sb;
    char *mapped;
 
 
    /* 打开文件 */
    if ((fd = open(argv[1], O_RDWR)) < 0) {
        perror("open");
    }
 
    /* 获取文件的属性 */
    if ((fstat(fd, &sb)) == -1) {
        perror("fstat");
    }
 
    /* 将文件映射至进程的地址空间 */
    if ((mapped = (char *)mmap(NULL, sb.st_size, PROT_READ | 
                    PROT_WRITE, MAP_SHARED, fd, 0)) == (void *)-1) {
        perror("mmap");
    }
 
    /* 映射完后, 关闭文件也可以操纵内存 */
    close(fd);
 
    printf("%s", mapped);
 
    /* 修改一个字符,同步到磁盘文件 */
    mapped[20] = '9';
    if ((msync((void *)mapped, sb.st_size, MS_SYNC)) == -1) {
        perror("msync");
    }
 
    /* 释放存储映射区 */
    if ((munmap((void *)mapped, sb.st_size)) == -1) {
        perror("munmap");
    }
 
    return 0;
}
