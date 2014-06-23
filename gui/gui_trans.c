#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "../util/die.h"
#include <unistd.h>

#define BUFF_NUM 2

static int _sd;
static pthread_mutex_t sd_mutex;

void init_gui_trans(int sd){
    int val = 1; //non block
    ioctl(sd, FIONBIO, &val);
    pthread_mutex_init(&sd_mutex,NULL);
    _sd = sd;
}

void gui_send(int x,int y){
    int buf[BUFF_NUM];
    buf[0] = x; buf[1] = y;
    pthread_mutex_lock(&sd_mutex);
    if(send(_sd, buf, sizeof(int)*BUFF_NUM,0) < 0){
        if(errno != EAGAIN)
            die("sendto");
    }
    pthread_mutex_unlock(&sd_mutex);
}

int* gui_recv(){
    static int buf[BUFF_NUM];
    int i;
    pthread_mutex_lock(&sd_mutex);
    if((i = recv(_sd, buf, sizeof(int)*BUFF_NUM, 0)) < 0){
        if(errno != EAGAIN)
            die("recvfrom");
    }
    pthread_mutex_unlock(&sd_mutex);

    if(i != BUFF_NUM*sizeof(int)) return NULL;
    return buf;
}
