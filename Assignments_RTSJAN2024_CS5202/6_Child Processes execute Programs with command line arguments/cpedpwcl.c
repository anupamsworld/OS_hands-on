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
	// this for loop is to traverse through the command line arguments of this program
	for(int cnt=1 ; cnt<=argc-1 ; cnt++){
		printf("\nGoing to execute %s\n",argv[cnt]);
		
		
		char pathToExe[200];
		//sprintf(pathToExe,"./%s",argv[cnt]);
		sprintf(pathToExe,"%s",argv[cnt]);
			
		char nestedArg[200];
		char *argumentsForNestedProgram[3];
		
		//checking if at all argument for executable programs or not
		if( (cnt<=argc-2) && (argv[cnt+1][0]=='-') ){
			//printf("\nargv[cnt+1][0]=%c\n",argv[cnt+1][0]);
			//printf("\nhere!!!\n");
			argumentsForNestedProgram[0] = pathToExe ;
			argumentsForNestedProgram[1] = argv[cnt+1] ;
			argumentsForNestedProgram[2] = NULL ;
			cnt++;
		}
		else{
			//printf("\nthere!!!\n");
			argumentsForNestedProgram[0] = pathToExe;
			argumentsForNestedProgram[1] = NULL ;
		}
		pid=fork();
		
		if(pid==0){
			
			//printf("\npathToExe=%s\n",pathToExe);
			
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
	return 0;
}

/*
Sample command to run cpedp.c= ./a.out ./pf -if /bin/ls -l ./pf2 -machine
pf. pf2 are executable files which are created by compiling program_file.c and program_file_2.c repectively
Apart from own program, this program accepts executable files of system also. For example "ls"
*/
