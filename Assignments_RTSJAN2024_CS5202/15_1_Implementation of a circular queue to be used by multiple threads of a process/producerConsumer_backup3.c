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

int producerNumber=0, consumerNumber=0;






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

/*noOfFreeSpacesQ() is not thread safe. call only if mutex is locked*/
int noOfFreeSpacesQ(){
	if(front==-1)
		return QUEUE_SIZE;
	else if(rear >= front)
		return (front+QUEUE_SIZE-1-rear) ;
	else
		return (front-rear-1) ;
}

/*enQ() is not thread safe. call only if mutex is locked*/
void enQ(int numItems) {
	int item;
	for (int i = 0; i < numItems; ++i) {
		if (front == -1)
			front = rear = 0;
		else
			rear = (rear + 1) % QUEUE_SIZE;
		item = rand() % 100;  // Generate random item
		queue[rear] = item;
		printf("Produced item: %d\n", item);
	}
}

/*deQ() is not thread safe. call only if mutex is locked*/
void deQ(int numItems) {
	for (int i = 0; i < numItems; ++i) {
		int item = queue[front];
		printf("Consumed item: %d\n", item);
		if (front == rear) front = rear = -1;
		else front = (front + 1) % QUEUE_SIZE;
	}
}

void *producer(void *data) {
	int numItems;
	int oldstate;
	int ret;
	while (1) {
		pthread_testcancel();
		ret=pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
		pthread_mutex_lock(&mutex);
		
		if (noOfFreeSpacesQ() == 0) {
			printf("Queue is full, cannot add item.\n");
			pthread_cond_wait(&full, &mutex);
		}
		printf("\nat producer:: noOfFreeSpacesQ()=%d\n",noOfFreeSpacesQ());
		numItems = (rand() % noOfFreeSpacesQ()) + 1;  // Generate random number of items to produce
		
		printf("\nProducer Thread %d\n",*(int *)data);
		printf("-----Going to produce %d items-----\n", numItems);
		enQ(numItems);
		printf("-----%d items are produced-----\n",numItems);

		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&mutex);
		ret=pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
		pthread_testcancel();

		usleep(500000);  // Sleep for 0.5 seconds for readability
	}
	return NULL;
}

void *consumer(void *data) {
	int numItems;
	int oldstate;
	int ret;
	while (1) {
		pthread_testcancel();
		ret=pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
		pthread_mutex_lock(&mutex);
		
		if (noOfFreeSpacesQ() == QUEUE_SIZE) {
			printf("Queue is empty, cannot consume\n");
			pthread_cond_wait(&empty, &mutex);
		}
		printf("\nat consumer:: noOfFreeSpacesQ()=%d\n",noOfFreeSpacesQ());
		numItems = (rand() % (QUEUE_SIZE-noOfFreeSpacesQ())) + 1;  // Generate random number of items to consume
		
		printf("\nConsumer Thread %d\n",*(int *)data);
		printf("-----Going to consume %d items-----\n",numItems);
		deQ(numItems);
		printf("-----%d items are consumed-----\n", numItems);
		
		pthread_cond_signal(&full);
	    	pthread_mutex_unlock(&mutex);
	    	ret=pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
		pthread_testcancel();
	    	
		usleep(500000);  // Sleep for 0.5 seconds for readability
		
	}
	return NULL;
}
int showOptions(){
	
	printf("\nManager Menu:\n");
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
            		producerNumber=numProducers+1;
                	pthread_create(&producerThreads[numProducers], NULL, producer, &producerNumber);
                	pthread_detach(producerThreads[numProducers]);
                	
                	/* deferred mode */
                	int oldtype;
			if(pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype)){
				printf("\nError while setting cencel type.\n");
			}
			
                	numProducers++;
                }
                else{
                	printf("Maximum possible number of Producers is reached. Can't add more.\n");
                }
                
                printf("\n<< Any time, Press ctrl+z to stop the program and ctrl+c to get manager menu >>\n");
                
                break;
            }
            case 2: {
            	if (numConsumers < QUEUE_SIZE) {
            		consumerNumber=numConsumers+1;
                	pthread_create(&consumerThreads[numConsumers], NULL, consumer, &consumerNumber);
                	pthread_detach(producerThreads[numConsumers]);
                	
                	/* deferred mode */
                	int oldtype;
			if(pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldtype)){
				printf("\nError while setting cencel type.\n");
			}
                	
                	numConsumers++;
                }
                else{
                	printf("\nMaximum possible number of Consumers is reached. Can't add more.\n");
                }
                
                printf("\n<< Any time, Press ctrl+z to stop the program and ctrl+c to get manager menu >>\n");
                
                break;
            }
            case 3: {
                if (numProducers > 0) {
                	int producer_cancel_status;
                    	producer_cancel_status=pthread_cancel(producerThreads[--numProducers]);
                    	if(producer_cancel_status==0)
                    		printf("\nProducer deleting request is placed successfully.\n");
                    	else
                    		printf("\nProducer deletion request could not be placed.\n");
                    		
                	printf("\n<< Any time, Press ctrl+z to stop the program and ctrl+c to get manager menu >>\n");
                }
                else {
                    printf("No producers to delete.\n");
                    status=0;
                }
                break;
            }
            case 4: {
                if (numConsumers > 0) {
			int consumer_cancel_status;
                    	consumer_cancel_status=pthread_cancel(consumerThreads[--numConsumers]);
                    	if(consumer_cancel_status==0)
                    		printf("\nConsumer deletion request is placed successfully.\n");
                    	else
                    		printf("\nConsumer deletion request could not be placed.\n");
                    		
                	printf("\n<< Any time, Press ctrl+z to stop the program and ctrl+c to get manager menu >>\n");
                }
                else {
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
	
	pthread_mutex_lock(&mutex);  // Lock mutex to prevent interference with producer/consumer threads
	
	while(1){
        	if(showOptions()==1)
			break;
        }

        pthread_mutex_unlock(&mutex);  // Unlock mutex to allow other threads to execute
    
}
int main() {


	signal(SIGINT, releaseSHM);
	printf("\n<< Any time, Press ctrl+z to stop the program and ctrl+c to get manager menu >>\n");
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




