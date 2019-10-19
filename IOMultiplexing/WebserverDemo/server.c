/*server.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/epoll.h>

#define	MAXLINE		80
#define SERV_PORT	8000
#define MAXEPOLLSIZE	10000
#define LISTENOP	1024

int make_socket_non_blocking(int sfd)
{

	
	if (fcntl(sfd, F_SETFL, fcntl(sfd, F_GETFD, 0)|O_NONBLOCK) == -1) {
		return -1;
	}
	return 0;
}



int main(void)
{
	struct sockaddr_in servaddr, cliaddr;
	socklen_t cliaddr_len;
	int listenfd, connfd;
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];
	int i, n;
	
	//for epoll
	int efd, curfds, nfds;
	struct epoll_event ev;
	int count;
	int acceptCount = 0;
	//for epoll monitor queue
	struct epoll_event events[MAXEPOLLSIZE];		
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons (SERV_PORT);
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) 
	{
		perror("can't create socket file");
		return -1;
	}
	
	int opt = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	cliaddr_len = sizeof(cliaddr);

	
	if (make_socket_non_blocking(listenfd) < 0) {
		perror("setnonblock error");
	}
	
	
	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) 
	{
		perror("bind error");
		return -1;
	} 
	
	if (listen(listenfd, LISTENOP) == -1) 
	{
		perror("listen error");
		return -1;
	}

	efd = epoll_create(MAXEPOLLSIZE);
	ev.events = EPOLLIN | EPOLLET;
    	ev.data.fd = listenfd;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &ev) < 0) 
    	{
        	fprintf(stderr, "epoll set insertion error: fd=%d\n", listenfd);
		return -1;
    	}
	
	curfds = 1;
	
	printf("epollserver startup,port %d, max connection is %d, backlog is %d\n", 
				SERV_PORT, MAXEPOLLSIZE, LISTENOP);
	
	printf("Accepting connections .....\n");

	while (1)
	{	
		/*wait for client*/
		nfds = epoll_wait(efd, events, curfds, -1);		
		if (nfds == -1)
        {
            perror("epoll_wait");
            continue;
        }
		printf("*****************%d\n", nfds);
		
		for(count = 0; count < nfds; ++count)
		{
			printf("+++++++++++++%d\n", count);				
			
			if (events[count].data.fd == listenfd) 
			{				
				connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&cliaddr_len);
				if (connfd < 0) 
                {
                    perror("accept error");
                    continue;
                }
				
				sprintf(buf, "accept form %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
                printf("%d:%s", ++acceptCount, buf);
				
				if (curfds >= MAXEPOLLSIZE) {
                    fprintf(stderr, "too many connection, more than %d\n", MAXEPOLLSIZE);
                    close(connfd);
                    continue;
                }
				
				if (make_socket_non_blocking(connfd) < 0) {
                    perror("setnonblocking error");
                }
				
				ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = connfd;
				
				if (epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &ev) < 0)
                {
                    fprintf(stderr, "add socket '%d' to epoll failed: %d\n", connfd, -1/*strerror(errno)*/);
                    return -1;
                }
				
				curfds++;
				printf("*************************\n");
                continue;
								
			}
			if (handle(events[count].data.fd) < 0) 
			{
				epoll_ctl(efd, EPOLL_CTL_DEL, events[count].data.fd,&ev);
				curfds--;            	
			}

		}
	}
	
	close(listenfd);
    return 0;
}
int handle(int connfd) 
{
	
    int nread, i;
    char buf[MAXLINE];
    nread = read(connfd, buf, MAXLINE);//读取客户端socket流

    if (nread == 0) {
        printf("client close the connection\n");
        close(connfd);
        return -1;
    } 
    if (nread < 0) {
        perror("read error");
        close(connfd);
        return -1;
    }
	
	for (i = 0; i < strlen(buf); i++)
		buf[i] = toupper(buf[i]);
	
    write(connfd, buf, nread);//响应客户端  
    return 0;
}
