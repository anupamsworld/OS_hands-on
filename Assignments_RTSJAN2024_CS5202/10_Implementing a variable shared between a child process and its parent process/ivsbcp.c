#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>

#include<errno.h>

#include<signal.h>
/* function to check through the characters of a given string and return 1 if all characters are numeric, else 0(zero) */
int isOnlyNumber(const char *s)
{
	while (*s) {
		if (isdigit(*s++) == 0) return 0;
	}

    return 1;
}

/* function to calculate factorial of a given number */
unsigned long long int getFactorialValue(unsigned long long int number){
	unsigned long long int i,fact=1 ;
	if(number==0)
		return fact;
	for(i=1;i<=number;i++){    
		fact=fact*i;    
	}
	return fact;
}

struct sharedMemoryDataTye{
	unsigned long long int lastFactorialResult;
};

int shmid;
pid_t pid, parent_pid;
//int pid;

void manipulateSharedMemory(int sigNum){

	printf("\nReceived signal=%d\n",sigNum);
	if(sigNum == SIGUSR1){
		//Parent sent this signal, Child is handling
		
		
		printf("\nChild process id=%d\n",getpid());
			
		// Attach to the shared memory segment
		unsigned long long int *shmaddr = shmat(shmid, NULL, 0);
		if (shmaddr == (unsigned long long int *)-1) {
			perror("Error in shmat call: ");
			exit(1);
		}
		
		
			
		printf("\nChild process P%d!\nCalculating the factorial of %llu \n", getpid(), *shmaddr);
		
		*shmaddr = getFactorialValue( *shmaddr );
		
		printf("\nFactorial calculation is done\n");
		printf("\nFactorial value from child=%llu\n", *shmaddr);

		// Detach shared memory from the child process
		if (shmdt(shmaddr) == -1) {
		    perror("Error in shmdt call: ");
		    exit(EXIT_FAILURE);
		}
		
		
		
		kill(parent_pid, SIGUSR2);
		
		pause();
	}
	else if(sigNum == SIGUSR2){
		//Child sent this signal, Parent is handling
		
		
		
		// Attach to the shared memory segment
		unsigned long long int *shmaddr = shmat(shmid, NULL, 0);
		if (shmaddr == (unsigned long long int *)-1) {
			perror("Error in shmat call: ");
			exit(1);
		}
		printf("\nChild process has confirmed that it has saved the calculated factorial value. Now in parent.\n");
		
		printf("\nFactorial result = %llu\n", *shmaddr);
		
		*shmaddr = rand() % 20 + 1 ;
		
		// Detach shared memory from the child process
		if (shmdt(shmaddr) == -1) {
		    perror("Error in shmdt call: ");
		    exit(EXIT_FAILURE);
		}
		
		kill(pid, SIGUSR1);
		
		pause();
		
	}
}

int main(int argc, char *argv[]){

	unsigned long long int n;
	
	parent_pid = getpid();
	printf("\nStating main process, having process id=%d\n", parent_pid);
	
 	int status;
	
	
	
	char * path;
	key_t key;
	printf("\ngetcwd(path,200)=%s\n",getcwd(path,200));
	if(key=ftok(getcwd(path,200),13)){
		printf("\nkey=%d\n",key);
	}
	else{
		printf("\nUnique key for getting shared memory could not be generated.\nAborting.\n");
	}
	
	
	size_t sizeOfSharedMemory=sizeof(n);
	
	
	// Create or get an already crated shared memory segment
	if ((shmid = shmget(key, sizeOfSharedMemory, IPC_CREAT | 0600)) < 0) {
		perror("Error in shmget call in main process: ");
		printf("\nError number: %d\n",errno);
		exit(1);
	}
	
	
	
	
	
	
	//shmaddr->lastFactorialResult = getFactorialValue( (n1==0 ? 0 : (n1-1)) ) ;
	
	
	
	printf("\nRAND_MAX: %i\n", RAND_MAX);
	
	
	
	//for(int i=0 ; i < 10 ; i++){
		//sighandler_t shandler ;
		pid=fork();
		
		if(pid==0){
		
			
			/* install signal handler */
			// sighandler_t signal(int signum, sighandler_t handler);
			
			//shandler =  signal(SIGUSR1, manipulateSharedMemory);
			signal(SIGUSR1, manipulateSharedMemory);
			
			
			
		}
		else if(pid==-1){
			printf("\nChild process could not be created.\nAborting.\n");
		}
		else{
			/* install signal handler */
			// sighandler_t signal(int signum, sighandler_t handler);
			
			//shandler =  signal(SIGUSR2, manipulateSharedMemory);
			signal(SIGUSR2, manipulateSharedMemory);
			
			
			
		}
	//}
	
	
	
	
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
	    perror("Error in shmctl");
	    exit(EXIT_FAILURE);
	}
	
	printf("\n\n");
	
	return 0;
}
