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

// #define DEFAULT_ADDR "fe80::e1c:22be:8e1c:c041"
// #define DEFAULT_ADDR "2a00:1398:9:fb03:90e8:e977:7164:22ef"
#define DEFAULT_ADDR "::1"
#define DEFAULT_ADDR_IP4 "172.17.68.144"
#define DEFAULT_PORT 1234

void print_ipv6(struct sockaddr_in6 *a) {
    char *buf;
    
    buf = malloc(sizeof(char)*INET6_ADDRSTRLEN);
    printf("%s p %i",inet_ntop(AF_INET6,&a->sin6_addr,buf, sizeof(char)*INET6_ADDRSTRLEN), a->sin6_port);
    printf("\n");
    
    free(buf);
    
    return;
}


int open_connection() {
    int err, sock = 0;
    struct sockaddr_in remote;

    sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

    remote.sin_family = AF_INET;
    remote.sin_port = htons(DEFAULT_PORT);

    inet_aton(DEFAULT_ADDR_IP4, &remote.sin_addr);
    
    err = connect(sock, (struct sockaddr *) &remote, sizeof(remote));
    if (err<0) {
        perror ("connect");
        exit (EXIT_FAILURE);
    }

    return sock;
}

int open_connection_ipv6() {
    int err, sock = 0;
    struct sockaddr_in6 remote;

    sock = socket (AF_INET6, SOCK_STREAM, 0);
    if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

    remote.sin6_family = AF_INET6;
    remote.sin6_port = htons(DEFAULT_PORT);
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
        currsock = open_connection_ipv6();
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

struct color {
    char r,g,b;
};

struct pixel {
    int x,y;
    struct color col;
};

#define COL_LEN 6
#define PX_LEN 2
#define MAX_XY 8
#define MSG_LEN COL_LEN+PX_LEN+MAX_XY+3+20
#define COL_OFFSET MAX_XY+PX_LEN+3

int pix(char *p, struct pixel *px) {
    memset(p,0,MSG_LEN);
    // sprintf(p, "PX %i %i rrggbb\n", px->x, px->y);
    // memcpy(p+COL_OFFSET, &px->col, COL_LEN);
    printf("sending: ");
    sprintf(p, "PX %i %i %i%i%i\n",px->x,px->y,px->col.r,px->col.g,px->col.b);
    printf(p);
}

int send_px(int sock, struct pixel *px) {
    int err;
    char *msg = malloc(MSG_LEN);
    printf("malloced\n");
    
    pix(msg, px);
    int n = strlen(msg);
    err = write(sock, msg, n+1);
    sleep(1);
    if (err < 0) {
        perror ("send");
        // printf("ERR %i\n",errno);
        exit (EXIT_FAILURE);
        if (errno == EPIPE) {
        } else{
            printf("ERR %i\n",errno);
            return -1;
        }
    }
    free(msg);
    // printf("send %i\n",err);
    
    return 0;
}

int draw_rect(int sock, int x, int y, int size) {
    struct color col = {.r=99,.g=55,.b=11};
    struct pixel px = {.x=x, .y=y, .col=col};

    for (int dx=x;dx<x+size;dx++) {
        for (int dy=y;dy<y+size;dy++) {
            if(send_px(sock, &px)<0){
                dy-=1;
                continue;
            }
            px.x=dx;
            px.y=dy;
        }
    }
    printf("Fin");
    return 0;
}

void handle_sig(int sig) {
    printf("SIG %i\n",sig);
    exit(sig);
}

#define PX_MSG_LEN 10
#define HELP_MSG_LEN 4
#define CON_LEN 1
int main(int argc, char **argv) {
    int err;
    int *connections;
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, handle_sig);
    signal(SIGTERM, handle_sig);
    printf("main\n");
    connections = malloc(sizeof(int)*CON_LEN);
    free(connections);
    
    err = open_connections(CON_LEN, connections);

    draw_rect(*connections, 17,10,100);

    close_connections(CON_LEN, connections);
    
}

