#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[]){
	/* Validation of number of arguments */
	if (argc < 2) {
		printf("\nCommand line argument is expected and mandatory but not provided.\nAborting.\n");
		return 1;
	}
	pid_t pid;
	// 
	for(int cnt=1 ; cnt<=argc-1 ; cnt++){
		printf("\nGoing to execute %s\n",argv[cnt]);
		
		pid=fork();
		if(pid==0){
			char pathToExe[200];
			sprintf(pathToExe,"./%s",argv[cnt]);
			//printf("\npathToExe=%s\n",pathToExe);
			char *argumentsForNestedProgram[] = {pathToExe,NULL};
			
			printf("\n");
			if (execve(pathToExe,argumentsForNestedProgram,NULL) == -1) {
				char customErrorMessage[400];
				sprintf(customErrorMessage,"\nExec Fails<%s>: ",pathToExe);
				perror (customErrorMessage);
			}
			exit(0);
		}
	}
	printf("\n");
}

/*
Sample command to run cpedp.c= ./a.out pf df dir/ program_file.c pf2
When in place of executable files the user provides
1) C program file then Error: Exec format error
2) A directory then Error: No such file or directory
3) A non-existent file then Error: No such file or directory
*/
