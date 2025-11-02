#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>

#include<errno.h>

#include<math.h>
#include<semaphore.h>
#include<fcntl.h>


#define SEMAPHORE_NAME "/my_semaphore"

/* function to check through the characters of a given string and return 1 if all characters are numeric, else 0(zero) */
int isOnlyNumber(const char *s)
{
	while (*s) {
		if (isdigit(*s++) == 0) return 0;
	}

    return 1;
}

/* function to calculate factorial of a given number */
int getFactorialValue(int number){
	int i,fact=1 ;
	if(number==0)
		return fact;
	for(i=1;i<=number;i++){    
		fact=fact*i;    
	}
	return fact;
}

struct sharedMemoryDataTye{
	unsigned long long int polynomialSum;
};



/*
we need to run same set of operations once for n-th to 1-st term of polynomial
and once when all the child processes are done working we need to execute same set of operations for the last process/term(0-th term)
--as per the question
*/
void processTermByChildProcess(int argc, char *argv[], pid_t pid, int shmid, int term, int count, sem_t * sem){
	//printf("\nprocessTermByChildProcess__term=%d\n",term);
	if(pid==0){
		
		printf("\nChild process id=%d\n",getpid());
		
		
		//Calculate the i-th term's value
		long long int term_value=atoll(argv[count]) * (long long int)powl(atoll(argv[1]), term);
		
		
		
		// Attach to the shared memory segment
		struct sharedMemoryDataTye * shmaddr_child = shmat(shmid, NULL, 0);
		if (shmaddr_child == (struct sharedMemoryDataTye *)-1) {
			perror("Error in shmat call: ");
			exit(1);
		}
		
		// Wait on semaphore
		sem_wait(sem);
		
		shmaddr_child->polynomialSum = shmaddr_child->polynomialSum + term_value ;
		
		unsigned long long int polynomialSum_temp =  shmaddr_child->polynomialSum;
		
		// Release semaphore
		sem_post(sem);
		
		if(term == 0)
			printf("\nSum of polynomial=%llu\n",polynomialSum_temp);
		
		// Detach shared memory from the child process
		if (shmdt(shmaddr_child) == -1) {
		    perror("Error in shmdt call: ");
		    exit(EXIT_FAILURE);
		}
		
		exit(0);
	}
	
	
}

int main(int argc, char *argv[]){

	/* Validation of number of arguments */
	if (argc < 3) {
		printf("\nRequired number of arguments are not mentioned.\nAborting.\n");
		return 1;
	}
	/*
	else if(argc >= 3){
		printf("\nArguments are [%s], [%s], [%s]\n", argv[0], argv[1], argv[2]);
	}
	*/
	
	
	
	// Validation of input type 
	for(int cnt=1 ; cnt<=argc-1 ; cnt++){
		if(isOnlyNumber(argv[cnt])==0){
			printf("\nArgument number %d is expected to be a number[>=0] but something else is provided.\nAborting.\n",cnt);
			return 1;
		}
		else{
			//printf("\nArgument number %d is OK\n",cnt);
		}
	}
	
	
	
	
	
	
	//printf("\nStating main process, having process id=%d\n",getpid());
	
 	int status;
	pid_t pid;
	int shmid;
	
	char * path=malloc(200);
	
	key_t key;
	
	if(getcwd(path,200)){
		printf("\ngetcwd(path,200)=%s\n",getcwd(path,200));
	}
	else{
		perror("\nError in getcwd(path,200): \n");
	}
	
	if(key=ftok(getcwd(path,200),13)){
		printf("\nkey=%d\n",key);
	}
	else{
		printf("\nUnique key for getting shared memory could not be generated.\nAborting.\n");
	}
	
	
	size_t sizeOfSharedMemory=sizeof(struct sharedMemoryDataTye);
	
	
	// Create or get an already crated shared memory segment
	if ((shmid = shmget(key, sizeOfSharedMemory, IPC_CREAT | 0600)) < 0) {
		perror("Error in shmget call in main process: ");
		printf("\nError number: %d\n",errno);
		exit(1);
	}
	
	
	
	// Attach to the shared memory segment
	struct sharedMemoryDataTye * shmaddr = shmat(shmid, NULL, 0);
	if (shmaddr == (struct sharedMemoryDataTye *)-1) {
		perror("Error in shmat call");
		exit(1);
	}
	
	
	
	
	// Control the shared memory segment using shmctl
	struct shmid_ds shmbuffer;
	if (shmctl(shmid, IPC_STAT, &shmbuffer) == -1) {
		perror("Error in shmctl call: ");
		exit(1);
	}
	
	
	//-------------------------------------
	
	sem_t *sem;
	
	// Create and initialize semaphore
	sem = sem_open(SEMAPHORE_NAME, O_CREAT , 0600, 1);
	if (sem == SEM_FAILED){
		perror("sem_open");
		exit(EXIT_FAILURE);
	}
	//int term=argc-3,count=2 ; term >= 0 && count <= argc ; term--,count++
	int term, count;
	for(term=argc-3,count=2 ; term >= 1 && count <= argc-2 ; term--,count++){
		pid=fork();
		if(pid==0)
			processTermByChildProcess(argc, argv, pid, shmid, term, count, sem);
		else if(pid==-1){
			printf("\nChild process could not be created.\nAborting.\n");
		}
		else{
			//
		}
	}
	
	
	if(pid>0){
	
		//now waiting for all the child processes to complete their execution and terminate normally
		while ((pid = waitpid(-1, &status, 0)) > 0) {
			if (WIFEXITED(status)) {
			    printf("\nChild with PID %d exited with status %d\n", pid, WEXITSTATUS(status));
			}
			else{
			    printf("\nChild with PID %d terminated abnormally\nAborting\n", pid);
			    exit(1);
			}
		}
		//printf("\ngetpid()=%d__term=%d__count=%d__argc=%d\n", getpid(), term, count, argc);
		
		//now we will consider the last term(0-th term) of polinomial and calculate the final sum of the polynomial
		if(term==0 && count == (argc-1) ){
			pid=fork();
			if(pid==0)
				processTermByChildProcess(argc, argv, pid, shmid, term, count, sem);
			
		}
	}
	
	
	
	// Detach shared memory from the main process
	if (shmdt(shmaddr) == -1) {
	    perror("Error in shmdt call: ");
	    exit(EXIT_FAILURE);
	}
	
	if (shmctl(shmid, IPC_RMID, NULL) == -1) {
	    perror("Error in shmctl");
	    exit(EXIT_FAILURE);
	}
	
	
	printf("\n\n");
}
