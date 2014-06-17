#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
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
    if(argc != 2){
        printf("argument\n");
        return -1;
    }
    int sd;
    struct sockaddr_in addr;
    struct sockaddr_in from_addr;
    char buf[N];
    int i;
    socklen_t sin_size = sizeof(struct sockaddr_in);

    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    // 送信先アドレスとポート番号を設定する
    // 受信プログラムと異なるあて先を設定しても UDP の場合はエラーにはならない
    addr.sin_family = AF_INET;
    addr.sin_port = htons(50000);
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    printf("%s\n",inet_ntoa(addr.sin_addr));

    int fd;
    if((fd = open("/dev/dsp",O_WRONLY|O_CREAT|O_TRUNC|O_RDONLY)) < 0){
        perror("open");
        return -1;
    }
    
    while(1){
        read(fd,buf,sizeof(char)*N);
        // パケットをUDPで送信
        if(sendto(sd, buf, sizeof(char)*N, 0,
                  (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("sendto");
            return -1;
        }
        
        if((i = recvfrom(sd, buf, sizeof(buf), 0,
                         (struct sockaddr *)&from_addr, &sin_size)) < 0) {
            perror("recvfrom");
            return -1;
        }
        write(fd,buf,i);
    }
    close(sd);


    return 0;
}
