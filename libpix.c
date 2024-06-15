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

#define PX_MSG_LEN 10
#define HELP_MSG_LEN 4
#define CON_LEN 1 //TODO enable Con multiplexing?


// struct color {
//     char r,g,b;
// };

// struct pixel {
//     int x,y;
//     struct color col;
// };

#define COL_LEN 6
#define PX_LEN 2
#define MAX_XY 8
#define MSG_LEN COL_LEN+PX_LEN+MAX_XY+3+20
#define COL_OFFSET MAX_XY+PX_LEN+3
#define MAX_ERR 200

int pix(char *p, struct pixel *px) {
    sprintf(p, "PX %i %i %02d%02d%02dFF\n",px->x,px->y,px->col.r,px->col.g,px->col.b);
    // printf(p);
}

int noerr;
int send_px(int sock, struct pixel *px, char *msgbuf) {
    int err;
    
    pix(msgbuf, px);
    int n = strlen(msgbuf);
    err = write(sock, msgbuf, n);
    if (err < 0) {
        perror ("send");
        if (errno == EPIPE) {
            noerr +=1;
            if (noerr > MAX_ERR) {exit(EXIT_FAILURE);}
            return -1; //TODO consider fd dead, handle
        }
        exit (EXIT_FAILURE);

    }
    return 0;
}

int draw_rect(int sock, int x, int y, int size, char *msgbuf) {

    int r = rand()%99; 
    int g = rand()%99;
    int b = rand()%99;
    struct color col = {.r=r,.g=g,.b=b};
    struct pixel px = {.x=x, .y=y, .col=col};

    for (int dx=x;dx<x+size;dx++) {
        for (int dy=y;dy<y+size;dy++) {
            int *con = malloc(sizeof(int));
            if(send_px(sock, &px, msgbuf)<0){
                dy-=1;
                continue;
            }
            px.x=dx;
            px.y=dy;
        }
    }
    // printf("Fin\n");
    return 0;
}