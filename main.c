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


#define DEFAULT_ADDR "::1"
#define DEFAULT_ADDR_IP4 "127.0.0.1"
#define DEFAULT_PORT 1337

#define DEFAULT_WIDTH  500
#define DEFAULT_HEIGHT 500

#define PX_MSG_LEN 10
#define HELP_MSG_LEN 4
#define CON_LEN 1 //TODO enable Con multiplexing?

//GLOBAL VARS FOR NOW
int port = DEFAULT_PORT;
// int noerr = 0;


struct arguments {
    char *argz;
    size_t argz_len;
    //canvas size
    int width;
    int height;
    int delay; //rate limit
    int mode; //TODO
    //connection
    int port;
    char *ip;
    //TODO connection multiplexing
};


struct arguments args;
const char *argp_program_version = "version 0.5";
const char argp_program_doc[] = "Pixelflut client\v"
                                "More info: https://github.com/defnull/pixelflut/";

static const struct argp_option argp_program_opts_doc[] = {
        {"width", 'w', "width", 0, "Width"},
        {"height", 'h', "height", 0, "Height"},
        {"port", 'p', "port", 0, "Port"},
        {},
};


static error_t parse_arg(int key, char *arg, struct argp_state *state) {
    struct arguments *args = state->input;

    switch (key) {
        case 'w':
            if (arg != NULL) {
                args->width = atoi(arg);
            } 
            if (args->width == 0) {
                args->width = DEFAULT_WIDTH;
            }
            break;
        case 'h':
            if (arg != NULL) {
                args->height = atoi(arg);
            }
            if (args->height == 0) {
                args->height = DEFAULT_HEIGHT;
            }
            break;
        case 'p':
            if (arg != NULL) {
                args->port = atoi(arg);
            } else {
                args->port = DEFAULT_PORT;
            }
            break;
        case ARGP_KEY_ARG: return 0;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static const struct argp argp = {
        .options = argp_program_opts_doc,
        .parser = parse_arg,
        .doc = argp_program_doc,
};



// void print_ipv6(struct sockaddr_in6 *a) {
//     char *buf;
    
//     buf = malloc(sizeof(char)*INET6_ADDRSTRLEN);
//     printf("%s p %i",inet_ntop(AF_INET6,&a->sin6_addr,buf, sizeof(char)*INET6_ADDRSTRLEN), a->sin6_port);
//     printf("\n");
    
//     free(buf);
    
//     return;
// }


// int open_connection() {
//     int err, sock = 0;
//     struct sockaddr_in remote;

//     sock = socket (AF_INET, SOCK_STREAM, 0);
//     if (sock < 0)
//     {
//       perror ("socket");
//       exit (EXIT_FAILURE);
//     }

//     remote.sin_family = AF_INET;
//     remote.sin_port = htons(port);
//     printf("Connect %i\n",port);
//     inet_aton(DEFAULT_ADDR_IP4, &remote.sin_addr);
    
//     err = connect(sock, (struct sockaddr *) &remote, sizeof(remote));
//     if (err<0) {
//         perror ("connect");
//         exit (EXIT_FAILURE);
//     }

//     return sock;
// }

// int open_connection_ipv6() {
//     int err, sock = 0;
//     struct sockaddr_in6 remote;

//     sock = socket (AF_INET6, SOCK_STREAM, 0);
//     if (sock < 0)
//     {
//       perror ("socket");
//       exit (EXIT_FAILURE);
//     }
//     printf("port%i\n",port);
//     remote.sin6_family = AF_INET6;
//     remote.sin6_port = htons(port);
//     inet_pton(AF_INET6, DEFAULT_ADDR, &remote.sin6_addr);
    
//     print_ipv6(&remote);

//     err = connect(sock, (struct sockaddr *) &remote, sizeof(remote));
//     if (err < 0) {
//         perror ("connect");
//         exit (EXIT_FAILURE);
//     }

//     return sock;
// }


// int open_connections(int n, int *connections) {
//     int currsock;
//     memset(connections, 0, sizeof(int)*n);

//     for (int *con = connections; con-connections < sizeof(int)*n; con+=sizeof(int)) {
//         currsock = open_connection();
//         if (currsock < 0) {
//             goto cleanup;
//         }
//         *con = currsock;
//         printf("opened fd {%i}\n",*con);
//     }
//     return 0;

//     cleanup:
//     for (int *con = connections; con-connections < sizeof(int)*n; con+=sizeof(int)) {
//         close(*con);
//     }
//     return -1;
// }

// void close_connections(int n, int *connections) {
//     for (int *con = connections; con-connections < sizeof(int)*n; con+=sizeof(int)) {
//         printf("free fd {%i}\n",*con);

//         shutdown(*con, SHUT_WR);
//         close(*con);
//     }
// }

// struct color {
//     char r,g,b;
// };

// struct pixel {
//     int x,y;
//     struct color col;
// };

// #define COL_LEN 6
// #define PX_LEN 2
// #define MAX_XY 8
// #define MSG_LEN COL_LEN+PX_LEN+MAX_XY+3+20
// #define COL_OFFSET MAX_XY+PX_LEN+3

// int pix(char *p, struct pixel *px) {
//     sprintf(p, "PX %i %i %02d%02d%02dFF\n",px->x,px->y,px->col.r,px->col.g,px->col.b);
//     // printf(p);
// }

// char *msgbuf;
// int send_px(int sock, struct pixel *px) {
//     int err;
    
//     pix(msgbuf, px);
//     int n = strlen(msgbuf);
//     err = write(sock, msgbuf, n);
//     if (err < 0) {
//         perror ("send");
//         if (errno == EPIPE) {
//             noerr +=1;
//             if (noerr > MAXERR) {exit(EXIT_FAILURE);}
//             return -1; //TODO consider fd dead, handle
//         }
//         exit (EXIT_FAILURE);

//     }
//     return 0;
// }

// int draw_rect(int sock, int x, int y, int size) {

//     int r = rand()%99; 
//     int g = rand()%99;
//     int b = rand()%99;
//     struct color col = {.r=r,.g=g,.b=b};
//     struct pixel px = {.x=x, .y=y, .col=col};

//     for (int dx=x;dx<x+size;dx++) {
//         for (int dy=y;dy<y+size;dy++) {
//             int *con = malloc(sizeof(int));
//             if(send_px(sock, &px)<0){
//                 dy-=1;
//                 continue;
//             }
//             px.x=dx;
//             px.y=dy;
//         }
//     }
//     // printf("Fin\n");
//     return 0;
// }

char *msgbuf;
int running = 1;
void handle_sig(int sig) {
    printf("SIG %i\n",sig);
    //TODO handle TERM
    running = 0;
}

int main(int argc, char **argv) {
    int err;
    int *connections;
    
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, handle_sig);
    signal(SIGTERM, handle_sig);
    
    struct arguments args;

    args.width = DEFAULT_WIDTH;
    args.height = DEFAULT_HEIGHT;
    args.port = DEFAULT_PORT;

    err = argp_parse(&argp, argc, argv, 0, 0, &args);
    if (err != 0)
        return err;
    
    port = args.port;

    connections = malloc(sizeof(int)*CON_LEN);
    msgbuf = malloc(MSG_LEN);
    err = open_connections(CON_LEN, connections);

    while(running){
        int x =rand()%args.width;
        int y = rand()%args.height;
        // printf("Rect %i,%i\n",x,y);
        draw_rect(*connections, x,y, 100, msgbuf);
    }

    free(msgbuf);
    close_connections(CON_LEN, connections);
    
}

