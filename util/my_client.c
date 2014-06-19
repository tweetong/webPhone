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
static struct sockaddr_in from_addr;
static socklen_t sin_size = sizeof(struct sockaddr_in);

void init_my_client(char *argv){
    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) die("socket");

// 送信先アドレスとポート番号を設定する
// 受信プログラムと異なるあて先を設定しても UDP の場合はエラーにはならない
    addr.sin_family = AF_INET;
    addr.sin_port = htons(50000);
    addr.sin_addr.s_addr = inet_addr(argv);

//printf("%s\n",inet_ntoa(addr.sin_addr));

/*
  ここで、ノンブロッキングに設定しています。
  val = 0でブロッキングモードに設定できます。
  ソケットの初期設定はブロッキングモードです。
*/
    int val = 1;
    ioctl(sd, FIONBIO, &val);
}

void my_client_send(char buf[],unsigned int size){
    if(sendto(sd,buf,size,0,(struct sockaddr*)&addr,sizeof(addr)) < 0){
        if(errno != EAGAIN)
            die("client sendto");
    }
}

int my_client_recv(char buf[]){
    int i;
    if((i = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&from_addr, &sin_size)) < 1){
        if(errno != EAGAIN)
            die("client recvfrom");
    }
    return i;
}

void my_client_close(){
    close(sd);
}
