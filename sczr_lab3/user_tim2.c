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

    if ((regs = (WzTim1Regs*)mmap(NULL, sizeof(WzTim1Regs), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        printf("Mapping failed.\n");
        return 1;
    }

    // turn off interrupts
    regs->stat = 0;

    // load timer period
    regs->divh = period >> 32;
    regs->divl = period & 0xffffffff;

    for (i = 1; i <= loop_count; i++)
    {
        // wait until 31th bit is set (busy-waiting)
        while((regs->stat & 0x80000000) != 0x80000000)
            ;

        // read from registers time passed
        time_passed = regs->cntl;
        time_passed = time_passed + (regs->cnth << 32);

        printf("%u %lu\n", i, time_passed);

        // tell timer we got informed about last tic
        regs->cntl = 0x0;
    }

    // turn off timer
    regs->divh = 0x0;
    regs->divl = 0x0;

    // clean up
    munmap(NULL, sizeof(WzTim1Regs));
    close(fd);
}


