#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#define N 100
int main(int argc, char** argv)
{
    int sd;
    struct sockaddr_in addr;

    socklen_t sin_size = sizeof(struct sockaddr_in);
    struct sockaddr_in from_addr;

    char buf[N]; // 受信バッファ
    int i;
    // IPv4 UDP のソケットを作成
    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    // 待ち受けるIPとポート番号を設定
    addr.sin_family = AF_INET;
    addr.sin_port = htons(50000);
    addr.sin_addr.s_addr = INADDR_ANY; // すべてのアドレス宛のパケットを受信する

    // バインドする
    if(bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    // 受信バッファの初期化
    memset(buf, 0, sizeof(buf));

    int fd;
    if((fd = open("/dev/dsp",O_WRONLY|O_CREAT|O_TRUNC,0644)) < 0){
        perror("open");
        return -1;
    }

    while(1){
        
        // 受信 パケットが到着するまでブロック
        // from_addr には、送信元アドレスが格納される
        if((i = recvfrom(sd, buf, sizeof(buf), 0,
                         (struct sockaddr *)&from_addr, &sin_size)) < 0) {
            perror("recvfrom");
            return -1;
        }
        if(write(fd,buf,i) < 0){
            perror("write");
            return -1;
        }
        /* if(sendto(sd, "I am send process", 17, 0, */
        /*           (struct sockaddr *)&from_addr, sizeof(from_addr)) < 0) { */
        /*     perror("sendto"); */
        /*     return -1; */
        /* } */
    }

    // ソケットのクローズ
    close(sd);
    close(fd);
    // 受信データの出力

    return 0;
}
