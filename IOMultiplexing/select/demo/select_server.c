#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <netinet/in.h> 
#include <sys/time.h> 
#include <sys/ioctl.h> 
#include <unistd.h> 
#include <stdlib.h>

int main()
{
    int server_sockfd, client_sockfd;
    int server_len, client_len;

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    int result;
    fd_set readfds, testfds;
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(8888);

    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr*)&server_address, server_len);
    listen(server_sockfd, 5);

    FD_ZERO(&readfds);
    FD_SET(server_sockfd, &readfds);

    while(1){
        char ch;
        int fd;
        int nread;
        testfds = readfds;

        printf("server waiting\n");

        //FD_SETSIZE：系统默认的最大文件描述符
        result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0 , (struct timeval *)0);
        if(result < 1){
            perror("server 5");
            exit(1);
        }

        for(fd = 0; fd < FD_SETSIZE; fd++){
            if(FD_ISSET(fd, &testfds))
            {
                /*判断是否为服务器套接字，是则表示为客户请求连接。*/
                if(fd == server_sockfd)
                {
                    client_len = sizeof(client_address);
                    client_sockfd = accept(server_sockfd, (struct sockaddr*)& client_address, &client_len);
                    FD_SET(client_sockfd, &readfds);
                    printf("add client on fd %d\n", client_sockfd);                    
                }else{      /*客户端socket中有数据请求时*/
                    ioctl(fd, FIONREAD, &nread);

                    /*客户数据请求完毕，关闭套接字，从集合中清除相应描述符 */
                    if(nread == 0){
                        close(fd);
                        FD_CLR(fd, &readfds);
                        printf("removing client on fd %d\n", fd);
                    }else{  /*处理客户数据请求*/
                        read(fd, &ch, 1);
                        sleep(5);
                        printf("serving client on fd %d %c\n", fd, ch);
                        ch++;
                        write(fd, &ch, 1);
                    }
                }
            }
        }

    }

    return 0;
}