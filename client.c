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

#include "util/die.h"
#include "util/params.h"
#include "util/init.h"
#include "util/my_trans.h"
#include "gui/gui_send.h"
#include "gui/gui_recv.h"

#include <X11/X.h>
#include <X11/Xlib.h>

int main(int argc, char** argv)
{
    if(argc != 2){
        printf("argument\n");
        exit(1);
    }
    XInitThreads();

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

    void *_gui_recv(void* arg){
        gui_tcp_recv(argc,argv,tcp_sd);
        return NULL;
    }
    pthread_t gui_c_recv_th;
    pthread_create(&gui_c_recv_th,NULL,_gui_recv,NULL);
    
    
    //for udp
    int sd;
    struct sockaddr_in addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) die("socket");
    // 送信先アドレスとポート番号を設定する
    // 受信プログラムと異なるあて先を設定しても UDP の場合はエラーにはならない
    addr.sin_family = AF_INET;
    addr.sin_port = htons(50000);
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    //printf("%s\n",inet_ntoa(addr.sin_addr));
    /*
      ここで、ノンブロッキングに設定しています。
      val = 0でブロッキングモードに設定できます。
      ソケットの初期設定はブロッキングモードです。
    */
    int val = 1;
    ioctl(sd, FIONBIO, &val);

    int fd;
    if((fd = open("/dev/dsp",O_RDWR)) < 0) die("open");
    init(fd);

    pthread_mutex_t fd_mutex;
    pthread_mutex_init(&fd_mutex,NULL);
    pthread_mutex_t sd_mutex;
    pthread_mutex_init(&sd_mutex,NULL);
    pthread_t send_th;
    pthread_t recv_th;

    MY_THREAD_ARG send_th_arg;
    send_th_arg.sd = sd;
    send_th_arg.fd = fd;
    send_th_arg.sin_size = sin_size;
    send_th_arg.fd_mutex = &fd_mutex;
    send_th_arg.sd_mutex = &sd_mutex;
    send_th_arg.addr = &addr;
    MY_THREAD_ARG recv_th_arg;
    recv_th_arg.sd = sd;
    recv_th_arg.fd = fd;
    recv_th_arg.sin_size = sin_size;
    recv_th_arg.fd_mutex = &fd_mutex;
    recv_th_arg.sd_mutex = &fd_mutex;
    
    pthread_create(&send_th,NULL,mysend,&send_th_arg);
    pthread_create(&recv_th,NULL,myrecv,&recv_th_arg);
    
    pthread_join(send_th,NULL);
    pthread_join(recv_th,NULL);
    pthread_join(gui_c_send_th,NULL);
    pthread_join(gui_c_recv_th,NULL);
    
    close(sd);
    close(fd);
    close(tcp_sd);

    return 0;
}
