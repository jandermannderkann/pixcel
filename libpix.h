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

int pix(char *p, struct pixel *px);

int send_px(int sock, struct pixel *px, char *msgbuf);

int draw_rect(int sock, int x, int y, int size, char *msgbuf);