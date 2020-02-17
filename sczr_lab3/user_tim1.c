// SCZR 2018 Ex 3, demo code by WZab
// Sources of the data generator
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <wzab_tim1.h>

int main(int argc, char *argv[])
{
    int i;
    int fd;
    //[...] To be filled with your code

    
    uint64_t period;         // nanoseconds 
    uint64_t cur_time;
    int loops_count;

    if (argc != 3)
    {
        printf("Usage: %s period loops_count\n", argv[0]);
        return 0;
    } else {
        period = atoi(argv[1]);
        loops_count = atoi(argv[2]);
    }
    
    if ((fd = open("/dev/my_tim0", O_RDWR | O_SYNC)) < 0)
    {
        printf("Failed to open 'my_tim0' device.\n");
        return 1;
    }

    write(fd, &period, 8);

    for (i = 1; i <= loops_count; i++)
    {
        read(fd, &cur_time, 8);
        printf("%d %d\n", i, cur_time);
    }

    // no need to turn timer off
    // driver takes care of it
    // time = 0;
    // write(fd, &time, sizeof(time));
    close(fd);
}

