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

#include "die.h"
#include "my_trans.h"
#include "params.h"
#include "fft.h"

void *mysend(void *arg){
    MY_THREAD_ARG *my_thread_arg =(MY_THREAD_ARG*)arg;
    char *buf;
    buf = (char*)calloc( sizeof(char), N);
    while(1){
        pthread_mutex_lock(my_thread_arg->fd_mutex);
        if((read(my_thread_arg->fd,buf,sizeof(char)*N)) < 0) die("read");
        pthread_mutex_unlock(my_thread_arg->fd_mutex);
        int i;
	for(i = 0; i<512 ; i++){
	  filter(buf,(int)((float)N/512.0));
	}  
        // パケットをUDPで送信
        pthread_mutex_lock(my_thread_arg->sd_mutex);
        if(sendto(my_thread_arg->sd, buf, sizeof(char)*N, 0, (struct sockaddr *)(my_thread_arg->addr), sizeof(struct sockaddr_in)) < 0){
            if(errno != EAGAIN)
                die("sendto");
        }
        pthread_mutex_unlock(my_thread_arg->sd_mutex);
    }
    return NULL;
}

void *myrecv(void *arg){
    MY_THREAD_ARG *my_thread_arg =(MY_THREAD_ARG*)arg;
    char buf[N];
    int i;
    struct sockaddr_in from_addr;
    while(1){
        pthread_mutex_lock(my_thread_arg->sd_mutex);
        if((i = recvfrom(my_thread_arg->sd, buf, sizeof(buf), 0, (struct sockaddr *)&from_addr, &(my_thread_arg->sin_size))) < 1){
            if(errno != EAGAIN)
                die("recvfrom");
        }
        pthread_mutex_unlock(my_thread_arg->sd_mutex);

        if(i >= 0){
            //ここでbufを逆変換する
            pthread_mutex_lock(my_thread_arg->fd_mutex);
            if((write(my_thread_arg->fd,buf,i)) < 0) die("write");
            pthread_mutex_unlock(my_thread_arg->fd_mutex);
        }
    }
    return NULL;
}
