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

#include "die.h"
#include "my_trans.h"
#include "params.h"
#include "fft.h"

void *mysend(void *arg){
    MY_THREAD_ARG *my_thread_arg =(MY_THREAD_ARG*)arg;
    char *buf;
    char *raw_buf;
    int i;    
    buf = (char*)calloc( sizeof(char), N);
    raw_buf = (char*)calloc( sizeof(char), N);
    
    //buf初期化
    pthread_mutex_lock(my_thread_arg->fd_mutex);
    if((read(my_thread_arg->fd,raw_buf,sizeof(char)*ONCE_SEND)) < 0) die("read");
    pthread_mutex_unlock(my_thread_arg->fd_mutex);
    for(i = 0;i < N;i++) buf[i] = raw_buf[i];
    
    while(1){
        /* memmove(raw_buf,raw_buf + ONCE_SEND,N - ONCE_SEND); */
        /* memmove(buf,buf + ONCE_SEND,N - ONCE_SEND); */
        
        pthread_mutex_lock(my_thread_arg->fd_mutex);
        if((read(my_thread_arg->fd,buf /* + N - ONCE_SEND */,sizeof(char)*ONCE_SEND)) < 0) die("read");
        pthread_mutex_unlock(my_thread_arg->fd_mutex);

        filter(buf,N);
        // パケットをUDPで送信
	pthread_mutex_lock(my_thread_arg->sd_mutex);
        if(sendto(my_thread_arg->sd, buf, sizeof(char)*ONCE_SEND, 0, (struct sockaddr *)(my_thread_arg->addr), sizeof(struct sockaddr_in)) < 0){
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
