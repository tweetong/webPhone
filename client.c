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

#include "util/die.h"
#include "util/params.h"

int main(int argc, char** argv)
{
    if(argc != 2){
        printf("argument\n");
        exit(1);
    }
    
    int sd;
    struct sockaddr_in addr;
    struct sockaddr_in from_addr;
    char buf[N];
    int i;
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

    while(1){
        if((read(fd,buf,sizeof(char)*N)) < 0) die("read");
        // パケットをUDPで送信
        if(sendto(sd, buf, sizeof(char)*N, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) die("sendto");

        if((i = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&from_addr, &sin_size)) < 1){
            if(errno != EAGAIN)
                die("recvfrom");
        }

        if((write(fd,buf,i)) < 0) die("write");
    }
    
    close(sd);
    close(fd);

    return 0;
}
