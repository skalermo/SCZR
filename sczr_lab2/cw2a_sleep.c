// SCZR 2018 Ex 2, demo code by WZab
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
#include "cw2a.h"

int main(int argc, char *argv[])
{
    int i;
    int fd;
    char line[200];
    pid_t cpids[MAX_CLIENTS];
    memset(cpids,0,sizeof(cpids));
    if (argc < 4) {
        fprintf(stderr, "Usage: %s number_of_clients number_of_samples sampling_period processing_delay\n", argv[0]);
        exit(EXIT_FAILURE);
    }    
    //Number of clients
    int ncli=atoi(argv[1]);
    if(ncli > MAX_CLIENTS) {
        printf("Number of clients must be below %d\n",(int) MAX_CLIENTS);
        exit(EXIT_FAILURE);
    }
    //Number of samples created
    int nsmp=atoi(argv[2]);
    //Sampling period in microseconds
    int udelsmp=atoi(argv[3]);
    int fout=open("server.txt",O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR);
    assert(fout>=0);
    //Create shared memory
    fd = shm_open(SHM_CW2_NAME, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    assert(fd>=0);
    //Initialize shared memory
    assert(ftruncate(fd, SHM_LEN) != -1);
    //Map the shared memory
    void * mptr = mmap(NULL, SHM_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    assert(mptr != MAP_FAILED);
    struct ringbuf * rbuf = (struct ringbuf *) mptr;
    //Prepare the shared memory
    /* Initialize the condition variable and its mutex */
    {
        pthread_condattr_t attr;
        pthread_condattr_init(&attr);
        pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        assert(pthread_cond_init(&rbuf->cvar,&attr)==0);
    }
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        assert(pthread_mutex_init(&rbuf->cvar_lock,&attr)==0);
    }
    //Everything went fine, so set the magick
    rbuf->magick = 0x12345678;
    //Now we start clients that should receive the data
    for(i=0; i<ncli; i++) {
        pid_t pid=fork();
        if(pid==0) {
            static char nr[5];
            sprintf(nr,"%5d",i);
            static char * cargv[]= {"cw2b",nr,NULL,NULL,NULL};
            cargv[2] = argv[2];
            cargv[3] = argv[4];
            execvp("cw2b",cargv);
            printf("I couldn't start cw2b client!\n");
            exit(127); //If execv fails
        } else {
            cpids[i] = pid;
        }
    }
    //Now we can start generating data and delivering data
    unsigned long prev_smptime = 0, accum = 0;
    struct timeval start, end, diff;
    diff.tv_usec = 0;
    for(i=0; i<nsmp; i++) {
        int j;
        unsigned long smptime;
	long wait = udelsmp - diff.tv_usec;
        usleep(wait < 0 ? 0 : wait);
	gettimeofday(&start, NULL);
        //Prepare data to be inserted
        pthread_mutex_lock(&rbuf->cvar_lock);
        //check if there is place for the new data
        int new_head = rbuf->head+1;
        if(new_head >= BUF_LEN) new_head = 0;
        for(j=0; j<ncli; j++) {
            if(new_head == rbuf->tail[j]) {
                //No place for new data!
                fprintf(stderr,"Client %d buffer overflow!\n",j);
                break;
            }
        }
        struct timeval tv1;
        gettimeofday(&tv1,NULL);
        memcpy(&rbuf->buf[rbuf->head].tstamp,&tv1,sizeof(tv1));
        //Here we should fill the data, but now we ignore it!
        rbuf->head = new_head;
        //Wake up consumers!
        pthread_cond_broadcast(&rbuf->cvar);
        pthread_mutex_unlock(&rbuf->cvar_lock);
        //Write "sampling time" to the file
        smptime=1000000*tv1.tv_sec+tv1.tv_usec;
        sprintf(line,"%d, %lu, %lu\n",i,smptime, smptime - prev_smptime);
        prev_smptime = smptime;
        //Please note, that this is not a correct implementation!
        //Now we only detect possible error, but we should also check the number of written bytes
        //and repeat writing if only part of the line was written?
        assert(write(fout,line,strlen(line))>0); 
        sync();
	gettimeofday(&end, NULL);
	timersub(&end, &start, &diff);
    }
    printf("waiting for children");
    for(i=0; i<ncli; i++) {
        if(cpids[i]) waitpid(cpids[i],0,0);
    }
    munmap(mptr,SHM_LEN);
    shm_unlink(SHM_CW2_NAME);
    close(fout);
}

