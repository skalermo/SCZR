#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define MSG_MAX_SIZE 100
#define NETCAT_HOST_PORT 10000
#define QUEUE_INITIALIZER {NULL, NULL, 0}
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t new_message_cond = PTHREAD_COND_INITIALIZER;

// representation of node in the list
struct Message {
    char* msg;
    struct Message* next;
    struct Message* prev;
};

struct Queue {
    struct Message* head;
    struct Message* tail;
    unsigned int size;
};

// arguments passed to threads
struct threadArguments {
    struct Queue* queue;
    int sockfd;
    unsigned int timeout;
};

//functions' signatures

void push(struct Queue*, char*);
char* popFirst(struct Queue*);

// no need to use
void cleanQueue(struct Queue*);

void* receiveMessages(void*);
void* sendMessages(void*);

int main(int argc, char* argv[]) {
//    runTests();

    // arguments to take:
    // argv[1] = hostname
    // argv[2] = timeout
    if (argc != 3) {
        printf("Usage: %s hostname timeout\n", argv[0]);
    }

    // Create Socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Select address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(NETCAT_HOST_PORT);
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Try to connect to socket
    printf("Connecting to server\n");
    if (connect(sockfd, &addr, sizeof(addr)) < 0) {
        printf("Connection Failed");
        return -1;
    }
    printf("Connected\n");

    pthread_t pid1, pid2;
    struct Queue queue = QUEUE_INITIALIZER;
    struct threadArguments args = {&queue, sockfd, atoi(argv[2])};

    pthread_create(&pid1, NULL, receiveMessages, &args);
    pthread_create(&pid2, NULL, sendMessages, &args);

    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);

    return 0;
}

void push(struct Queue* queue, char* message) {
    if (queue == NULL)
        return;
    struct Message* ptr_msg = (struct Message*)malloc(sizeof(struct Message));
    ptr_msg->msg = message;
    // if queue is empty
    if (queue->size == 0) {
        queue->head = ptr_msg;
        // if there is no guard
        if (queue->tail == NULL) {
            // create a guard on the end of the queue
            queue->tail = (struct Message*)malloc(sizeof(struct Message));
            queue->tail->msg = NULL;
            queue->tail->next = queue->tail;
        }
        queue->tail->prev = queue->head;
        ptr_msg->prev = queue->tail;
    } else {
        // if queue is not empty
        queue->tail->prev->next = ptr_msg;
        ptr_msg->prev = queue->tail->prev;
        queue->tail->prev = ptr_msg;
    }
    ptr_msg->next = queue->tail;
    queue->size++;
}

char* popFirst(struct Queue* queue) {
    if (queue == NULL)
        return NULL;
    if (queue->size == 0)
        return NULL;
    else {
        // if queue is not empty
        char* message = queue->head->msg;
        queue->size--;

        // if there was just one element
        if (queue->size == 0) {
            free(queue->head);
            queue->head = queue->tail;
            queue->tail->prev = queue->tail;
        }
        else {
            // if there was more than one element
            struct Message* ptr = queue->head;
            queue->head = queue->head->next;
            queue->head->prev = queue->tail;
            free(ptr);
        }
        return message;
    }
}

void* receiveMessages(void* arguments) {
    struct threadArguments* args = (struct threadArguments*)arguments;
    while (1) {
        char* input_msg = (char*)malloc(MSG_MAX_SIZE * sizeof(char));

        // get user input
        fgets(input_msg, MSG_MAX_SIZE, stdin);

        // push new message, send signal to waiting thread
        pthread_mutex_lock(&queue_mutex);
        push(args->queue, input_msg);
        pthread_cond_signal(&new_message_cond);
        pthread_mutex_unlock(&queue_mutex);
    }
}

void* sendMessages(void* arguments) {
    struct threadArguments* args = (struct threadArguments*)arguments;
    while (1) {
        pthread_mutex_lock(&queue_mutex);

        // wait for the signal
        while (args->queue->size == 0) {
            pthread_cond_wait(&new_message_cond, &queue_mutex);
        }

        // pop first message from the queue
        char* message = popFirst(args->queue);
        pthread_mutex_unlock(&queue_mutex);

        // simulate network traffic
        sleep(args->timeout);

        // send message via socket to netcat server
        if(strlen(message) > 0)
            write(args->sockfd, message, strlen(message));
    }
}
