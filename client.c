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
#include "util/init.h"
#include "util/my_client.h"


int main(int argc, char** argv)
{
    if(argc != 2){
        printf("argument\n");
        exit(1);
    }
    int recv_size;
    char buf[N];
    
    init_my_client(argv[1]);
    
    int fd = init_dsp_fd();
    
    while(1){
        if((read(fd,buf,sizeof(char)*N)) < 0) die("read");

        my_client_send(buf,sizeof(char)*N);

        recv_size = my_client_recv(buf);

        if(recv_size >= 0){
            if((write(fd,buf,recv_size)) < 0) die("write");
        }
    }
    
    my_client_close();
    close(fd);

    return 0;
}
