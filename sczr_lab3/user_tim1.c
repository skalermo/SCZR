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
    uint64_t time_passed;
    int loop_count;

    if (argc != 3)
    {
        printf("Usage: %s period loops_count\n", argv[0]);
        return 0;
    }

    period = strtoul(argv[1], NULL, 0);
    loop_count = atoi(argv[2]);
    
    if ((fd = open("/dev/my_tim0", O_RDWR | O_SYNC)) < 0)
    {
        printf("Failed to open 'my_tim0' device.\n");
        return 1;
    }

    write(fd, &period, 8);

    for (i = 1; i <= loop_count; i++)
    {
        read(fd, &time_passed, 8);
        printf("%u %lu\n", i, time_passed);
    }

    close(fd);
}

