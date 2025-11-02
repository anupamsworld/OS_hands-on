/* File: threadMutex2.c */
/* Many (nproducer no.) producer threads can produce data in sharedData (an integer variable)
   Many consumer (nconsumer no.) threads can consume from the sharedData (the integer variable)
   A producer thread cannot produce any data unless the earlier data produced by some
   producer thread is consumed by some consumer thread. Data produced by a producer thread can be consumed
   by only one consumer thread. */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> /* for thread functions */
#include <errno.h> /* For the macros used here - EAGAIN, EINVAL, EPERM. etc.*/

/* Compile and link with -pthread, for example "gcc -pthread threadMutex1.c" */

/* Program to demonstrate mutex locks in thread programming */

#include <unistd.h>
#include <signal.h>
#define QUEUE_SIZE 10

struct passedData {
	int *data;
	int producer_no;
	int consumer_no;
};
struct passedData *threadData;

int sharedData = 0;

void *producer( void *param);
void *consumer( void *param);


pthread_mutex_t p_mutex;
pthread_mutex_t c_mutex;








int queue[QUEUE_SIZE];
int front = -1, rear = -1;
int doesManagerWant=0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutexForManager = PTHREAD_MUTEX_INITIALIZER;

pthread_t producerThreads[10];
pthread_t consumerThreads[10];
int numProducers = 0, numConsumers = 0;

void enQ(int item) {
    pthread_mutex_lock(&mutex);
    if ((front == 0 && rear == QUEUE_SIZE - 1) ||
        (rear == (front - 1) % (QUEUE_SIZE - 1))) {
        printf("Queue is full, cannot add item %d\n", item);
        pthread_cond_wait(&full, &mutex);
    }

    if (front == -1)
    	front = rear = 0;
    else
    	rear = (rear + 1) % QUEUE_SIZE;

    queue[rear] = item;
    printf("Produced item: %d\n", item);
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);
}

int deQ() {
    pthread_mutex_lock(&mutex);
    if (front == -1) {
        printf("Queue is empty, cannot consume\n");
        pthread_cond_wait(&empty, &mutex);
    }

    int item = queue[front];
    printf("Consumed item: %d\n", item);
    if (front == rear) front = rear = -1;
    else front = (front + 1) % QUEUE_SIZE;

    pthread_cond_signal(&full);
    pthread_mutex_unlock(&mutex);
    return item;
}

void *producer(void *data) {
    int numItems;
    while (1) {
        numItems = rand() % 5 + 1;  // Generate random number of items to produce
        for (int i = 0; i < numItems; ++i) {
            int item = rand() % 100;  // Generate random item
            enQ(item);
            usleep(500000);  // Sleep for 0.5 seconds for readability
        }
    }
    return NULL;
}

void *consumer(void *data) {
    while (1) {
        int numItems = rand() % 5 + 1;  // Generate random number of items to consume
        for (int i = 0; i < numItems; ++i) {
            deQ();
            usleep(500000);  // Sleep for 0.5 seconds for readability
        }
    }
    return NULL;
}
int showOptions(){
	
	printf("\nManager Options:\n");
	printf("\nnumProducers=%d,\tnumConsumers=%d\n",numProducers,numConsumers);
        printf("1. Add Producer\n");
        printf("2. Add Consumer\n");
        printf("3. Delete Producer\n");
        printf("4. Delete Consumer\n");
        printf("Enter your choice: ");

        int choice;
        scanf("%d", &choice);

        int status=1;

        switch (choice) {
            case 1: {
            	if (numProducers < QUEUE_SIZE) {
                	pthread_create(&producerThreads[numProducers++], NULL, producer, NULL);
                }
                else{
                	printf("Maximum possible number of Producers is reached. Can't add more.\n");
                }
                break;
            }
            case 2: {
            	if (numConsumers < QUEUE_SIZE) {
                	pthread_create(&consumerThreads[numConsumers++], NULL, consumer, NULL);
                }
                else{
                	printf("Maximum possible number of Consumers is reached. Can't add more.\n");
                }
                break;
            }
            case 3: {
                if (numProducers > 0) {
                    pthread_cancel(producerThreads[--numProducers]);
                    printf("Producer deleted successfully.\n");
                } else {
                    printf("No producers to delete.\n");
                    status=0;
                }
                break;
            }
            case 4: {
                if (numConsumers > 0) {
                    pthread_cancel(consumerThreads[--numConsumers]);
                    printf("Consumer deleted successfully.\n");
                } else {
                    printf("No consumers to delete.\n");
                    status=0;
                }
                break;
            }
            default:{
                printf("Invalid choice!\n");
                status=0;
            }
        }

	return status;
        
}
void *manager(void *data) {
	while(1){
		usleep(10*1000000);
	}
}
typedef void (*sighandler_t)(int);
void releaseSHM(int signum) {
	doesManagerWant=1;
	
	pthread_mutex_lock(&mutex);  // Lock mutex to prevent interference with producer/consumer threads
	
	while(1){
        	if(showOptions()==1)
			break;
        }

        pthread_mutex_unlock(&mutex);  // Unlock mutex to allow other threads to execute
    
}
int main() {


	signal(SIGINT, releaseSHM);
	
	pthread_mutex_lock(&mutex);
	while(1){
        	if(showOptions()==1)
			break;
        }
        pthread_mutex_unlock(&mutex);
	
	srand(time(NULL));
	pthread_t managerThread;
	pthread_create(&managerThread, NULL, manager, NULL);
	

	pthread_join(managerThread, NULL);
	
	return 0;
	
	

}




