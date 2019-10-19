#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>

#define		PORTNUM		13000

int main(int ac, char *av[])
{
	int sock_id, sock_fd;
	FILE* sock_fp;
	struct sockaddr_in clntaddr;
	char buf[BUFSIZ], dirname[BUFSIZ];	
	
	if(ac != 3)
	{
		printf("usage: ./client ipaddress dirname\n");
		exit(1);
	}
	
	if((sock_id = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("crate socket fail\n");
		exit(2);
	}
	
	//initial ipv4
	bzero(&clntaddr, sizeof(clntaddr));
	clntaddr.sin_family = AF_INET;
	//clntaddr.sin_port = htons(atoi(av[2]));
	clntaddr.sin_port = htons(PORTNUM);
	
	if(connect(sock_id,(struct sockaddr*)&clntaddr, sizeof(clntaddr)) < 0)
	{
		printf("connect socket fail\n");
		exit(3);
	}
	
	int n = 0;
	{		
		n = recv(sock_id, buf, BUFSIZ, 0);
		printf("%s\n", buf);		
	}
	
	bzero(dirname, strlen(dirname));
	strcpy(dirname, av[2]);
	printf("ls %s:", dirname);
	
	struct timeval tmOut;      	
	tmOut.tv_sec = 0;
	tmOut.tv_usec = 0;
	fd_set         fds;
	FD_ZERO(&fds);
	FD_SET(sock_id, &fds);

	int nRet;

	//wait for write port is ready and the buffer is clear
	if(n > 0)
	while(1)
	{           
		nRet= select(FD_SETSIZE, &fds, NULL, NULL, &tmOut);
		//nRet = select(0, NULL,&fds,NULL,&tmOut);
		if(nRet == 0)  break;
		recv(sock_id, buf, 1, 0);
		//write(sock_id, (const void *)'\0', 1);

	}
	
	//write the directory name to port(socket stream)
	if(send(sock_id, dirname, strlen(dirname), 0) == -1)
	{
		printf("server send fail\n");
		exit(4);
	}
	
	bzero(buf, sizeof(buf));
	
	n = 0;
	{		
		n = recv(sock_id, buf, BUFSIZ, 0);
		printf("%s\n", buf);		
	}
	
	//printf("enter the file name to copy:");
	//memset(buf, '0', BUFSIZ);
	
	//scanf("%s", buf);
	
	close(sock_id);	
}