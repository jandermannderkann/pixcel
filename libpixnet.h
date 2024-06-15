#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <argp.h>
#include <argz.h>

#define PX_MSG_LEN 10
#define HELP_MSG_LEN 4
#define CON_LEN 1 //TODO enable Con multiplexing?


void print_ipv6(struct sockaddr_in6 *a);
    

int open_connection();

int open_connection_ipv6();
   

int open_connections(int n, int *connections);
   
void close_connections(int n, int *connections);