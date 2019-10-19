/*	socklib.c
 *	this file contains functions uesd losts when writing internet
 *	client/server programs. The two main functions here are:
 *
 *	int make_server_socket(portnum) returns a server socket
 *			return a server socket or -1 if error
 *
 *	int make_server_socket_q(portnum, backlog)
 *
 *	int connect_to_server(char *hostname, int portnum)
 *			retrun a connected sock or -1 if error
 */
 
#include <stdio.h>
#include <time.h>
#include "socklib.h"

#define HOSTLEN		256
#define BACKLOG		1

int make_server_socket_q(int, int);

int make_server_socket(int portnum)
{
	return make_server_socket_q(portnum, BACKLOG);	
}

int make_server_socket_q(int portnum, int backlog)
{
	struct sockaddr_in saddr;
	struct hostent	*hp;
	char hostname[HOSTLEN];
	int sock_id;
	
	sock_id = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_id == -1)
		return -1;
	
	/*build address and bind it to socket*/
	bzero((void*)&saddr, sizeof(saddr));
	gethostname(hostname, HOSTLEN);
	printf("the host name is %s\n", hostname);
	hp = gethostbyname(hostname);					/*get info about host*/
	
	bcopy((void*)hp->h_addr, (void*)&saddr.sin_addr, hp->h_length);
	saddr.sin_port = htons(portnum);				/*fill in socket port*/
	saddr.sin_family = AF_INET;
	if(bind(sock_id, (struct sockaddr*)&saddr, sizeof(saddr)) != 0)
		return -1;
	
	/*arrange for incoming calls*/
	if(listen(sock_id, backlog) != 0)
		return -1;
	return sock_id;	
}

int connect_to_server(char* host, int portnum)
{
	int sock;
	struct sockaddr_in servadd;						/*the number to call*/	
	struct hostent *hp;
	
	/*step 1:get a socket*/
	sock = socket(AF_INET, SOCK_STREAM, 0);			/*get a line*/
	if(sock == -1)
		return -1;
	
	/*step 2: connect to server*/
	bzero(&servadd, sizeof(servadd));
	hp = gethostbyname(host);
	if(hp == NULL)
		return -1;
	bcopy(hp->h_addr, (struct sockaddr*)&servadd.sin_addr, hp->h_length);
	
	servadd.sin_port = htons(portnum);
	servadd.sin_family = AF_INET;
	
	if(connect(sock, (struct sockaddr*)&servadd, sizeof(servadd)) != 0)
		return -1;
	return sock;
}

int talk_with_server(int fd)
{
	char buf[BUFSIZ];
	int n;
	
	n = read(fd, buf, BUFSIZ);
	write(1, buf, n);
	return n;
}

int process_request(int fd)
{
	/*
	time_t now;
	char *cp;
	
	time(&now);
	cp = ctime(&now);
	write(fd, cp, strlen(cp));
	*/
	if(fork() == 0)
	{
		dup2(fd ,1);
		close(fd);
		execlp("date", "date", NULL);
		oops("execlp date", 1);
	}
}

int send_message(int fd)
{
	char buf[BUFSIZ];
	int n;
	
	n = read(0, buf, BUFSIZ);
	printf("%s\n", buf);
	write(fd, buf, n);
	return n;
}

int get_message(int fd)
{
	char buf[BUFSIZ];
	int n, i;
	
	n = read(fd, buf, BUFSIZ);
	printf("%s\n", buf);
	i = n;
	while(i++)
	{
		buf[i] = toupper(buf[i]);
		if(i >= n)
			break;
	}	
	write(1, buf, n);
	return n;
}