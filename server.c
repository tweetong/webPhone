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

#include "util/die.h"
#include "util/params.h"
#include "util/init.h"

int main(int argc, char** argv)
{
    int sd;
    struct sockaddr_in addr;

    socklen_t sin_size = sizeof(struct sockaddr_in);
    struct sockaddr_in from_addr;

    char buf[N]; // 受信バッファ
    int i;
    // IPv4 UDP のソケットを作成
    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) die("socket");

    // 待ち受けるIPとポート番号を設定
    addr.sin_family = AF_INET;
    addr.sin_port = htons(50000);
    addr.sin_addr.s_addr = INADDR_ANY; // すべてのアドレス宛のパケットを受信する

    // バインドする
    if(bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) die("bind");

    // 受信バッファの初期化
    memset(buf, 0, sizeof(buf));

    if((i = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&from_addr, &sin_size)) < 0){
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
    
    while(1){
        
        // 受信 パケットが到着するまでブロック
        // from_addr には、送信元アドレスが格納される
        if((i = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr *)&from_addr, &sin_size)) < 1){
            if(errno != EAGAIN) //ノンブロック
                die("recvfrom2");
        }
        if(i >= 0){
            if(write(fd,buf,i) < 0) die("write");
        }

        if(read(fd,buf,N) < 0) die("read");

//        printf("%s\n",inet_ntoa(from_addr.sin_addr));
        if(sendto(sd, buf, sizeof(char)*N, 0, (struct sockaddr *)&from_addr, sizeof(from_addr)) < 0) die("sendto");
    }

    // ソケットのクローズ
    close(sd);
    close(fd);

    return 0;
}
