#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
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

int main(int argc, char** argv)
{
    int sd;
    struct sockaddr_in addr;

    socklen_t sin_size = sizeof(struct sockaddr_in);
    struct sockaddr_in from_addr;

    // IPv4 UDP のソケットを作成
    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) die("socket");

    // 待ち受けるIPとポート番号を設定
    addr.sin_family = AF_INET;
    addr.sin_port = htons(50000);
    addr.sin_addr.s_addr = INADDR_ANY; // すべてのアドレス宛のパケットを受信する

    // バインドする
    if(bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) die("bind");

    char buf[N];
    // clientが来るまで待つ
    if((recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&from_addr, &sin_size)) < 0){
        die("recvfrom1");
    }

    /*
      ここで、ノンブロッキングに設定しています。
      val = 0でブロッキングモードに設定できます。
      ソケットの初期設定はブロッキングモードです。
    */
    int val = 1;
    ioctl(sd, FIONBIO, &val);
    
    int fd;
    if((fd = open("/dev/dsp",O_RDWR,0644)) < 0) die("open");
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
    send_th_arg.addr = &from_addr;
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

    // ソケットのクローズ
    close(sd);
    close(fd);

    return 0;
}
