#include <stdio.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/epoll.h>  
#include <fcntl.h>
#include <errno.h>
#include <string.h>
  
#define MAX_EVENTS 10  
 
 
int main()  
{  
    // socket
    struct sockaddr_in servaddr;  
    short port = 5859;  
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  
    servaddr.sin_port = htons(port);
 
    if (connect(sockfd, (sockaddr *) &servaddr, sizeof(sockaddr_in)) < 0) {
        perror("connect fail");
        exit(EXIT_FAILURE);
    }
 
    const char* buf = "daiyudong";
 
    for (;;) {
        int len = (int)write(sockfd, buf, strlen(buf));
        if (len > 0) {
            printf("write len=%d\n", len);
        }
        sleep(1);
    }
}
