#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define		PORTNUM		13000

int main(int ac, char *av[])
{
	int sock_id, sock_fd;
	FILE* sock_fp, *pipe_fp;
	struct sockaddr_in servaddr;
	char buf[BUFSIZ], command[BUFSIZ];
	char c;
	
	
	if((sock_id = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("back value %d crate socket faile\n", sock_id);
		exit(1);
	}
	
	//initial ipv4
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORTNUM);
	
	//bind sock_id to host address
	if(bind(sock_id, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		printf("bind socket fail\n");
		exit(2);
	}
	
	//listen client
	if(listen(sock_id, 1) == -1)
	{
		printf("bind socket fail\n");
		exit(3);
	}
	
	//wait for client connect
	printf("======wait for client=====\n");	
	
	int n = 0;
	
	while(1)
	{		
		if((sock_fd = accept(sock_id, (struct sockaddr *)NULL, NULL)) == -1)
		{
			printf("accept socket fail\n");
			exit(4);
		}
		
		if((n = send(sock_fd, "connect ok!", 13, 0)) == -1)
		{
			printf("server send fail\n");
			exit(5);
		}	
	
	
	struct timeval tmOut;      	
	tmOut.tv_sec = 0;
	tmOut.tv_usec = 0;
	fd_set         fds;
	FD_ZERO(&fds);
	FD_SET(sock_id, &fds);

	int nRet;

	if(n > 0)
	while(1)
	{           
		nRet= select(FD_SETSIZE, &fds, NULL, NULL, &tmOut);
		//nRet = select(0, NULL,&fds,NULL,&tmOut);
		if(nRet == 0)  break;
		recv(sock_fd, buf, 1, 0);
		//write(sock_id, (const void *)'\0', 1);

	}
		
	bzero(buf, sizeof(buf));
		
	
		
		/* get dirname*/
		if(n > 0)
		{				
				
			n = recv(sock_fd, buf, BUFSIZ, 0);
			if(n == -1)
			{
				printf("accept socket fail\n");
				exit(6);
			}
			printf("%s\n", buf);				
		}
		
		/* get the dir*/
		sprintf(command, "ls %s", buf);
		printf("command: %s\n", command);
		
		if((pipe_fp = popen(command, "r")) == NULL)
		{
			printf("popen fail\n");
			exit(7);
		}	
		
		bzero(buf, sizeof(buf));
		int i = 0;
		/*transfer data */
		while((c = getc(pipe_fp)) != EOF)
		{
			buf[i] = c;
			if(buf[i] == '\n')
				buf[i] = ' ';
			i++;
		}
		buf[i] = '\0';
		
		printf("*************%s\n", buf);
		
		if((n = send(sock_fd, buf, strlen(buf), 0)) == -1)
		{
			printf("server send dir content fail\n");
			exit(8);
		}	
		
		close(sock_fd);
	}
	
}