// SCZR 2018 Ex 3, demo code by WZab
// Scheleton of the application
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
    WzTim1Regs * volatile regs;
    //[...] To be filled with your code


    // to make our life easier
    // we work only with lower half 
    // of timer's registers
    uint32_t period;         // nanoseconds 
    uint32_t cur_time;

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

    // 0x1000 is equivalent to 4096 bytes
    // which is size of 1 page unit (block)
    // it is the minimal memory size which can be allocated using mmap

    // 0x900a000 is an offset of our device's registers in memory
    if ((regs = (WzTim1Regs*)mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0x900a000)) == MAP_FAILED)
    {
        printf("Mapping failed.\n");
        return 1;
    }

    regs->divh = 0x0;
    regs->divl = time;

    for (i = 1; i <= loops_count; i++)
    {
        while(((regs->stat) & 0x80000000) == 0x80000000);
        cur_time = regs->cntl;
        printf("%d %d\n", i, cur_time);
        regs->cntl = 0x0;
    }

    regs->divl = 0x0;
    munmap(NULL, 0x1000);
    close(fd);
}

