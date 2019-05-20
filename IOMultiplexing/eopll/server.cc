#include <sys/socket.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE		10
#define MAX_EVENTS		500
#define MAX_LISTENED	5

const int port = 5859;

int createAndListen()
{
	int on = 1;
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in servaddr;
	fcntl(listenfd, F_SETFL, O_NONBLOCK);
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); //only enable debug!
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if(-1== bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)))
		printf("bind errno: %d\n", errno);

	if(-1==listen(listenfd, MAX_LISTENED))
		printf("listen errno: %d\n", errno);

	printf("listen in port: %d\n",port);
}

int main()
{
	struct epoll_event ev, events[MAX_EVENTS];
	int epollfd = epoll_create(1);

	if(epollfd < 0)
		printf("epoll_create error: %d\n", errno);

	int listenfd = createAndListen();
	ev.data.fd = listenfd;
	ev.events = EPOLLIN;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);

	for(;;)
	{
		int fds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if(fds == -1)
		{
			printf("epoll_wait errno: %d\n", errno);
			break;
		}
		else
			printf("trig %d!!!\n", fds);

		for(int i = 0; i < fds; i++)
		{
			if(events[i].data.fd = listenfd)
			{
				struct sockaddr_in clientaddr;
				socklen_t clientlen = sizeof(struct sockaddr_in);
				int connfd = accept(listenfd, (struct sockaddr*)&clientaddr, (socklen_t*)&clientaddr);
				if(connfd > 0)
					printf("new connection from %s:%d accept fd:%d\n", 
						inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), connfd);

				else
					printf("accept errno: %d, connfd%d\n", errno, connfd);

				fcntl(connfd, F_SETFL, O_NONBLOCK);
				ev.data.fd = connfd;
				ev.events = EPOLLIN|EPOLLET;
				if(-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev));
					printf("epoll_ctl errno %d\n", errno);				
			}
			else if(events[i].events & EPOLLIN)
			{
				int sockfd; 
				if(sockfd = events[i].data.fd < 0)
				{
					printf("EPOLLIN sock fd < 0, error\n");
					continue;
				}

				char szLine[MAX_LINE+1];
				bzero(szLine, MAX_LINE+1);
				int readLen = 0;
				
				if((readLen = read(sockfd, szLine, MAX_LINE)) < 0)
				{
					printf("readLen:%d errno:%d\n", readLen, errno);
					if(ECONNRESET == errno)
					{
						printf("ECONNRESET close socket fd: %d\n", events[i].data.fd);
						close(sockfd);
					}
				}
				else if(0 == readLen)
					printf("read 0 socket fd:%d\n", events[i].data.fd);
				else
					printf("read %d content:%s\n",readLen, szLine);

				bzero(szLine, MAX_LINE+1);
				if((readLen = read(sockfd, szLine, MAX_LINE)) < 0)
				{
					printf("readLen2 :%d errno:%d\n", readLen, errno);
					if(ECONNRESET == errno)
					{
						printf("ECONNRESET2 close socket fd: %d\n", events[i].data.fd);
						close(sockfd);
					}
				}
				else if(0 == readLen)
					printf("read2 0 socket fd:%d\n", events[i].data.fd);
				else
					printf("read2 %d content:%s\n",readLen, szLine);
			}
		}
	}

	printf("aa\n");
	return 0;
}
