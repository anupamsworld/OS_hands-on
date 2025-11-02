/*
 Let there be a (parent) process which goes on putting a random (integer) number  in a shared variable, say n,  in a regular interval. There is a child process that goes on putting the factorial of the integer (that it finds in n) back into the shared variable (n), in regular intervals. The parent process prints the factorial that it receives from the child process. Device your own mechanism(s) so that the child process "knows" that a new number has been put in n  by the parent process and the parent process "knows" that a the factorial of a number has been put in n  by the child  process,

Write a complete C program to implement the above. The program should make arrangements for releasing shared memory that has been created during execution.

The processes must print meaningful output so that the user understands what is happening.
*/



#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/shm.h>
#include <signal.h>

typedef void (*signalhandler_t)(int);
void controlSHM(int);
void calculateFactorial(int, int*);
int shmid;
pid_t pid;

int main() {
  signalhandler_t shandler;

  shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0777);
  if(shmid == -1) {
    perror("Error in shmget()!\n");
    exit(1);
  }



  pid = fork();
  if(pid == -1){
    perror("Error in fork()!\n");
    exit(1);
  }

  if(pid == 0) {
    //calculate factorial and put in the parent shared variable
    shandler = signal(SIGUSR1, controlSHM);
    //notify the parent that the factorial value has been updated in the shared memory

  }

  else {
    //put a random number in the shared variable and also print the factorial it receives from the child
    shandler = signal(SIGUSR2, controlSHM);

    //notify the child that a new randomNumber has been generated

    //print the factorial value

  }
}

void controlSHM (int signum) {

	int* sharedNum;
	sharedNum = shmat(shmid, NULL, 0);

	if (sharedNum == (int*)(void*)-1) {
	perror("shmat failed!\n");
	}

	if (signum == SIGUSR1) {
		// parent updated the shared variable with a new randomNumber
		
		sharedNum=getFactorialValue(sharedNum);
		kill(getppid(), SIGUSR2);
	}

	if (signum == SIGUSR2) {
		// child updated the shared variable with the factorial variable
		// int* bada;
		// bada = shmat(shmid, ...)
		
		sharedNum = rand() % 10 + 1 ;
		
		kill(pid, SIGUSR1);
	}

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












