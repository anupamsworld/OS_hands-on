#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h> /* for signal(2) kill(2) */

#define QUEUE_SIZE 10

typedef struct {
	int buffer[QUEUE_SIZE];
	int front, rear;
} CircularQueue;


int shmid;
int mutex, empty, full;
CircularQueue *queue;

/* following is a signal handler for the keypress ^C, that is, ctrl-c */
typedef void (*sighandler_t)(int);
void releaseSHM(int signum) {
        int status;
        // int shmctl(int shmid, int cmd, struct shmid_ds *buf); /* Read the manual for shmctl() */
        status = shmctl(shmid, IPC_RMID, NULL); /* IPC_RMID is the command for destroying the shared memory*/
        if (status == 0) {
                fprintf(stderr, "Remove shared memory with id = %d.\n", shmid);
        } else if (status == -1) {
                fprintf(stderr, "Cannot remove shared memory with id = %d.\n", shmid);
        } else {
                fprintf(stderr, "shmctl() returned wrong value while removing shared memory with id = %d.\n", shmid);
        }

        // int kill(pid_t pid, int sig);
        status = kill(0, SIGKILL);
        if (status == 0) {
                fprintf(stderr, "kill susccesful.\n"); /* this line may not be executed :P WHY?*/
        } else if (status == -1) {
                perror("kill failed.\n");
                fprintf(stderr, "Cannot remove shared memory with id = %d.\n", shmid);
        } else {
                fprintf(stderr, "kill(2) returned wrong value.\n");
        }
}

void produce(int items) {
	printf("\nProducing started...\n");
	struct sembuf sem_op;
	int item;
	
	sem_op.sem_num = 0;
	sem_op.sem_op = -items;
	sem_op.sem_flg = 0;
	semop(empty, &sem_op, 1);
	
	for(int i=items;i>0;i--){
		printf("\nEnter an integer to produce: ");
		scanf("%d", &item);
		printf("\nProducing...\n");
		
		sem_op.sem_num = 0;
		sem_op.sem_op = -1;
		semop(mutex, &sem_op, 1);
		

		// Add item to the queue
		queue->buffer[queue->rear] = item;
		queue->rear = (queue->rear + 1) % QUEUE_SIZE;

		sem_op.sem_num = 0;
		sem_op.sem_op = 1;
		semop(mutex, &sem_op, 1);
		
		printf("Produced\n");
		
	}
	
	sem_op.sem_num = 0;
	sem_op.sem_op = +items;
	semop(full, &sem_op, 1);
	
}

int main() {
	int item;

	char * path=malloc(200);
	
	key_t shm_key, sem_mutex_key, sem_empty_key, sem_full_key ;
	
	/* install signal handler */
        // sighandler_t signal(int signum, sighandler_t handler);
        signal(SIGINT, releaseSHM);
	
	if(getcwd(path,200)){
		printf("\ngetcwd(path,200)=%s\n",getcwd(path,200));
		if(shm_key=ftok(getcwd(path,200),1)){
			printf("\nshm_key=%d\n",shm_key);
		}
		else{
			printf("\nUnique key for getting shared memory could not be generated.\nAborting.\n");
			return 1;
		}
		
		if(sem_mutex_key=ftok(getcwd(path,200),2)){
			printf("\nsem_mutex_key=%d\n",sem_mutex_key);
		}
		else{
			printf("\nUnique key for getting shared memory could not be generated.\nAborting.\n");
			return 1;
		}
		
		if(sem_empty_key=ftok(getcwd(path,200),3)){
			printf("\nsem_empty_key=%d\n",sem_empty_key);
		}
		else{
			printf("\nUnique key for getting shared memory could not be generated.\nAborting.\n");
			return 1;
		}
		
		if(sem_full_key=ftok(getcwd(path,200),4)){
			printf("\nsem_full_key=%d\n",sem_full_key);
		}
		else{
			printf("\nUnique key for getting shared memory could not be generated.\nAborting.\n");
			return 1;
		}
	}
	else{
		perror("\nError in getcwd(path,200): \n");
		return 2;
	}
	
	
	
	
	// Create or access shared memory
	shmid = shmget(shm_key, sizeof(CircularQueue), IPC_CREAT | 0666);
	if (shmid < 0) {
		perror("shmget");
		exit(1);
	}
	queue = (CircularQueue *)shmat(shmid, NULL, 0);
	if (queue == (CircularQueue *)(-1)) {
		perror("shmat");
		exit(1);
	}
	
	queue->front = 0;
	queue->rear = 0;
	
	// Create or access semaphores
	mutex = semget(sem_mutex_key, 1, IPC_CREAT | 0666);
	if (mutex < 0) {
		perror("semget");
		exit(1);
	}
	empty = semget(sem_empty_key, 1, IPC_CREAT | 0666);
	if (empty < 0) {
		perror("semget");
		exit(1);
	}
	full = semget(sem_full_key, 1, IPC_CREAT | 0666);
	if (full < 0) {
		perror("semget");
		exit(1);
	}

	// Initialize semaphore values
	semctl(mutex, 0, SETVAL, 1);
	semctl(empty, 0, SETVAL, QUEUE_SIZE);
	semctl(full, 0, SETVAL, 0);
	int items=0;
	while (1) {
		items=0;
		while(1){
			printf("\nEnter how many items you want to produce at once[1-10]: ");
			scanf("%d", &items);
			if( (items >= 1) && (items <= QUEUE_SIZE) )
				break;
				
		}
		
		produce(items);
	}

	return 0;
}
