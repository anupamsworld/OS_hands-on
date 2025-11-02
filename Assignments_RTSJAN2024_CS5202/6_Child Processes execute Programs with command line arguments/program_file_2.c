
//This program fetches system related information

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include<sys/utsname.h>
#include<stdlib.h>
#include<string.h>


int main(int argc, char *argv[]) {
        printf("\nYou are in program_file_2.c---argv[1]=%s\n",argv[1]);
        
        struct utsname buf1; 
	errno =0; 
	if(uname(&buf1)!=0){ 
		perror("uname doesn't return 0, so there is an error"); 
		exit(EXIT_FAILURE); 
	} 
	
	
	
	
	//checking that if at all argument is provided
	if(argc>1){
        	
        	//print information as per the received arguments
		if(strcmp("-sys",argv[1])==0){
			printf("program_file_2.c :: System Name = %s\n", buf1.sysname); 
		}
		else if(strcmp("-node",argv[1])==0){
			printf("program_file_2.c :: Node Name = %s\n", buf1.nodename);
		}
		else if(strcmp("-version",argv[1])==0){
			printf("program_file_2.c :: Version = %s\n", buf1.version); 
		}
		else if(strcmp("-release",argv[1])==0){
			printf("program_file_2.c :: Release = %s\n", buf1.release);
		}
		else if(strcmp("-machine",argv[1])==0){
			printf("program_file_2.c :: Machine = %s\n", buf1.machine); 
		}
		else{
			printf("\nprogram_file_2.c :: argument is not known\n");
		}
        	
        	
        }
        
        return 0;

}
