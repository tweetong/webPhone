#include <fcntl.h>
#include <linux/soundcard.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "die.h"

int dsp_parameters_set(int fd){
    int rate = 48000;
    int bits = 16;

    /* サンプリングレート値を変更 */
    if(ioctl( fd, SOUND_PCM_WRITE_RATE, &rate ) < 0)
        die("ioctl( SOUND_PCM_READ_RATE )");
	
    /* 量子化ビット数を変更 */
    if(ioctl( fd, SOUND_PCM_WRITE_BITS, &bits ) < 0)
        die("ioctl( SOUND_PCM_READ_BITS )");
	
    return 0;
}
