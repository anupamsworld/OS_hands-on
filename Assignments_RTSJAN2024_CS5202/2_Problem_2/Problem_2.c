#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>

#include<errno.h>


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
	unsigned long long int lastFactorialResult;
};

int main(int argc, char *argv[]){

	/* Validation of number of arguments */
	if (argc < 3) {
		printf("\nN1 and N2 are not mentioned.\nAborting.\n");
		return 1;
	}
	else if(argc >= 3){
		printf("\nArguments are [%s], [%s], [%s]\n", argv[0], argv[1], argv[2]);
	}
	
	/* Validation of input type */
	if(isOnlyNumber(argv[1])==0){
		printf("\nN1 is a number[>=0] but something else is provided.\nAborting.\n");
		return 1;
	}
	if(isOnlyNumber(argv[2])==0){
		printf("\nN2 is a number[>=0] but something else is provided.\nAborting.\n");
		return 1;
	}
	int n1=atoi(argv[1]), n2=atoi(argv[2]);
	if( (n1 < 0) || (n1 >= n2) ){
		printf("\nN1 and N2 are not pertaining to given condition: (N1 > 0) AND (N1 < N2).\nAborting\n");
		exit(1);
	}
	
	
	printf("\nStating main process, having process id=%d\n",getpid());
	
 	int status;
	pid_t pid;
	//int pid;
	int shmid;
	
	char * path;
	key_t key;
	printf("\ngetcwd(path,200)=%s\n",getcwd(path,200));
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
	
	
	shmaddr->lastFactorialResult = getFactorialValue( (n1==0 ? 0 : (n1-1)) ) ;
	
	// Control the shared memory segment using shmctl
	struct shmid_ds shmbuffer;
	if (shmctl(shmid, IPC_STAT, &shmbuffer) == -1) {
		perror("Error in shmctl call: ");
		exit(1);
	}

	for(int i=n1 ; i <= n2 ; i++){
		pid=fork();
		
		if(pid==0){
		
			printf("\nChild process id=%d\n",getpid());
			
			// Attach to the shared memory segment
			struct sharedMemoryDataTye * shmaddr_child = shmat(shmid, NULL, 0);
			if (shmaddr_child == (struct sharedMemoryDataTye *)-1) {
				perror("Error in shmat call: ");
				exit(1);
			}
			
			if(i!=0)
				shmaddr_child->lastFactorialResult = shmaddr_child->lastFactorialResult * i ;
				
				
			printf("\nChild process P%d!\nFactorial of %d is %llu -- Using IPC.\n", i, i, shmaddr_child->lastFactorialResult);
			
			
			// Control the shared memory segment using shmctl
			struct shmid_ds shmbuffer_atChild;
			if (shmctl(shmid, IPC_STAT, &shmbuffer_atChild) == -1) {
				perror("Error in shmctl call: ");
				exit(1);
			}
			

			// Detach shared memory from the child process
			if (shmdt(shmaddr_child) == -1) {
			    perror("Error in shmdt call: ");
			    exit(EXIT_FAILURE);
			}
			
			exit(0);
		}
		else if(pid==-1){
			printf("\nChild process could not be created.\nAborting.\n");
		}
		else{
			int wait_status=waitpid(pid, &status, 0); //waiting for the child process Pi to complete its work.
			if( WIFEXITED(status)){
				printf("\nChild process P%d(%d) is done with executing. Now in parent.\n",i,wait_status);
			}
			else{
				printf("\nChild process has not terminated normally.\n");
				exit(1);
			}
			
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
