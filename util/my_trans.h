#ifndef __INCLUDE_MY_TRANS_H__
#define __INCLUDE_MY_TRANS_H__

typedef struct {
    int sd;
    int fd;
    socklen_t sin_size;
    pthread_mutex_t* fd_mutex;
    pthread_mutex_t* sd_mutex;
    struct sockaddr_in* addr;
} MY_THREAD_ARG;

void *mysend(void *arg);
void *myrecv(void *arg);

#endif
