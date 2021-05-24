//客户端
#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#define MAXLINE     1024
#define PORT        8787


void handle_recv_msg(int sockfd, char* buf)
{
    printf("client recv msg is: %s\n", buf);
    sleep(5);
    write(sockfd, buf, strlen(buf)+1);
}

void handle_connection(int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];
    int maxfdp, stdineof;
    fd_set readfds;
    struct timeval tv;

    while(1){
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        maxfdp = sockfd;

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int retval = select(maxfdp+1, &readfds, NULL, NULL, &tv);
        if(0 == retval){
            printf("client timeout.\n");
            continue;
        }

        if(FD_ISSET(sockfd, &readfds)){
            int n = read(sockfd, recvline, MAXLINE);
            if(n <= 0){
                fprintf(stderr, "client: server is close.\n");
                close(sockfd);
                FD_CLR(sockfd, &readfds);
                return;
            }

            handle_recv_msg(sockfd, recvline);
        }
    }
}

int main()
{
    int sockfd;        
    int len;
    struct sockaddr_in address;
    int result = 0;    

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);
    //inet_pton(AF_INET,IPADDRESS,&servaddr.sin_addr);

    len = sizeof(address);
    result = connect(sockfd, (struct sockaddr*) &address, len);

    if(result < -1){
        perror("oops: connect server fail!");
        exit(1);
    }


    printf("client send to server \n");
    write(sockfd, "connect to server", 32);    

    handle_connection(sockfd);

    return 0;
}