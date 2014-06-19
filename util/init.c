#include "dsp_set.h"
#include "die.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int init_dsp_fd(){
    int fd;
    if((fd = open("/dev/dsp",O_RDWR)) < 0) die("open");
    dsp_parameters_set(fd);
    return fd;
}

