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
#include "util/my_server.h"

int main(int argc, char** argv)
{
    char buf[N]; // 受信バッファ
    int recv_size;

    // 受信バッファの初期化
    memset(buf, 0, sizeof(buf));

    init_my_server();
    int fd = init_dsp_fd();
    
    while(1){
        recv_size = my_server_recv(buf);

        if(recv_size >= 0){
            if(write(fd,buf,recv_size) < 0) die("write");
        }

        if(read(fd,buf,N) < 0) die("read");

//        printf("%s\n",inet_ntoa(from_addr.sin_addr));
        my_server_send(buf,sizeof(char)*N);
    }

    // ソケットのクローズ
    my_server_close();
    close(fd);

    return 0;
}
