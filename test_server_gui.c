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
    int tcp_sd = socket(PF_INET,SOCK_STREAM,0);
    if(tcp_sd < 0) die("tcp_socket");
    struct sockaddr_in tcp_addr;
    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_addr.sin_port = htons(atoi(argv[1]));
    if(bind(tcp_sd,(struct sockaddr*)&tcp_addr,sizeof(tcp_addr)) < 0) die("tcp_bind");
    listen(tcp_sd,10);
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    int ss = accept(tcp_sd,(struct sockaddr*)&client_addr,&len);
    void *_gui_recv(void* arg){
        gui_tcp_recv(argc,argv,ss);
        return NULL;
    }

    pthread_t gui_s_recv_th;
    pthread_create(&gui_s_recv_th,NULL,_gui_recv,NULL);

    pthread_join(gui_s_recv_th,NULL);
    return 0;
}
