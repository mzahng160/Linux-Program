#ifndef		SOCKLIB_H_
#define		SOCKLIB_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
//#include <strings.h>
#include <string.h>
#include <stdlib.h>

#define 	oops(x,n)		{perror(x);exit(n);}

int make_server_socket(int);
int connect_to_server(char*, int);

int send_message(int );
int get_message(int );

#endif		//SOCKLIB_H_