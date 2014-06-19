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
#include "params.h"

static int sd;
static struct sockaddr_in addr;
static socklen_t sin_size = sizeof(struct sockaddr_in);
static struct sockaddr_in from_addr;

void init_my_server(){
    // IPv4 UDP のソケットを作成
    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) die("socket");

    // 待ち受けるIPとポート番号を設定
    addr.sin_family = AF_INET;
    addr.sin_port = htons(50000);
    addr.sin_addr.s_addr = INADDR_ANY; // すべてのアドレス宛のパケットを受信する

    // バインドする
    if(bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) die("bind");

    char buf[N];
    //誰か来るまで待つ
    if((recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&from_addr, &sin_size)) < 0){
        die("server recvfrom first");
    }

    /*
      ここで、ノンブロッキングに設定しています。
      val = 0でブロッキングモードに設定できます。
      ソケットの初期設定はブロッキングモードです。
    */
    int val = 1;
    ioctl(sd, FIONBIO, &val);
}

void my_server_send(char buf[],unsigned int size){
    if(sendto(sd,buf,size,0,(struct sockaddr*)&addr,sizeof(addr)) < 0){
        if(errno != EAGAIN)
            die("server sendto");
    }
}

int my_server_recv(char buf[]){
    int i;
    if((i = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&from_addr, &sin_size)) < 1){
        if(errno != EAGAIN)
            die("server recvfrom");
    }
    return i;
}

void my_server_close(){
    close(sd);
}
