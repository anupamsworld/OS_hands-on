#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>

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
	for(i=1;i<=number;i++){    
		fact=fact*i;    
	}
	return fact;
}


int main(int argc, char *argv[]){

	/* Validation of number of arguments */
	if (argc < 2) {
		printf("\nNumber of child processes to be created is not mentioned.\nAborting.\n");
		return 1;
	}
	else if(argc >= 2){
		printf("\nArguments are %s : %s...\n", argv[0], argv[1]);
	}
	
	/* Validation of input type */
	if(isOnlyNumber(argv[1])==0){
		printf("\nNumber of child processes to be created is a number but something else is provided.\nAborting.\n");
		return 1;
	}
	
	/* convert number argument string to integer */
	int n=atoi(argv[1]);
	
	pid_t pid;
	for(int i=0 ; i<n ; i++){
		pid=fork();
		
		if(pid==0){
			printf("\nChild process P%d!____Factorial of %d is %d\n", i+1, i+1, getFactorialValue(i+1));
			exit(0);
		}
	}
	
	printf("\n\n");
}
