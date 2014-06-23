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
#include <pthread.h>

#include "gui/gui_send.h"
#include "gui/gui_recv.h"
#include "util/die.h"


int main(int argc, char** argv)
{
    if(argc != 2){
        printf("argument\n");
        exit(1);
    }

    int tcp_sd = socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in tcp_addr;
    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_addr.s_addr = inet_addr(argv[1]);
    tcp_addr.sin_port = htons(50001);
    if(connect(tcp_sd,(struct sockaddr*)&tcp_addr,sizeof(struct sockaddr_in)) < 0 )
        die("connect");
    argc = 1;

    void *_gui_send(void* arg){
        gui_tcp_send(argc,argv,tcp_sd);
        return NULL;
    }
    pthread_t gui_c_send_th;
    pthread_create(&gui_c_send_th,NULL,_gui_send,NULL);
    pthread_join(gui_c_send_th,NULL);

    close(tcp_sd);
    return 0;
}
