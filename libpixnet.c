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
#include "libpix.h"
#include "libpixnet.h"

#define PX_MSG_LEN 10
#define HELP_MSG_LEN 4
#define CON_LEN 1 //TODO enable Con multiplexing?

#define DEFAULT_ADDR "::1"
#define DEFAULT_ADDR_IP4 "127.0.0.1"
#define DEFAULT_PORT 1337


void print_ipv6(struct sockaddr_in6 *a) {
    char *buf;
    
    buf = malloc(sizeof(char)*INET6_ADDRSTRLEN);
    printf("%s p %i",inet_ntop(AF_INET6,&a->sin6_addr,buf, sizeof(char)*INET6_ADDRSTRLEN), a->sin6_port);
    printf("\n");
    
    free(buf);
    
    return;
}

int open_connection(int port) {
    int err, sock = 0;
    struct sockaddr_in remote;

    sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

    remote.sin_family = AF_INET;
    remote.sin_port = htons(port);
    printf("Connect %i\n",port);
    inet_aton(DEFAULT_ADDR_IP4, &remote.sin_addr);
    
    err = connect(sock, (struct sockaddr *) &remote, sizeof(remote));
    if (err<0) {
        perror ("connect");
        exit (EXIT_FAILURE);
    }

    return sock;
}

int open_connection_ipv6(int port) {
    int err, sock = 0;
    struct sockaddr_in6 remote;

    sock = socket (AF_INET6, SOCK_STREAM, 0);
    if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }
    printf("port%i\n",port);
    remote.sin6_family = AF_INET6;
    remote.sin6_port = htons(port);
    inet_pton(AF_INET6, DEFAULT_ADDR, &remote.sin6_addr);
    
    print_ipv6(&remote);

    err = connect(sock, (struct sockaddr *) &remote, sizeof(remote));
    if (err < 0) {
        perror ("connect");
        exit (EXIT_FAILURE);
    }

    return sock;
}


int open_connections(int n, int *connections) {
    int currsock;
    memset(connections, 0, sizeof(int)*n);

    for (int *con = connections; con-connections < sizeof(int)*n; con+=sizeof(int)) {
        currsock = open_connection(DEFAULT_PORT);
        if (currsock < 0) {
            goto cleanup;
        }
        *con = currsock;
        printf("opened fd {%i}\n",*con);
    }
    return 0;

    cleanup:
    for (int *con = connections; con-connections < sizeof(int)*n; con+=sizeof(int)) {
        close(*con);
    }
    return -1;
}

void close_connections(int n, int *connections) {
    for (int *con = connections; con-connections < sizeof(int)*n; con+=sizeof(int)) {
        printf("free fd {%i}\n",*con);

        shutdown(*con, SHUT_WR);
        close(*con);
    }
}