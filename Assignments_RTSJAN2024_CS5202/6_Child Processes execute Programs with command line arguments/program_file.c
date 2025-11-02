
//This program prints some information basis on the input argument

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>



int main(int argc, char *argv[]) {
        printf("\nYou are in program_file.c---argv[1]=%s\n",argv[1]);
        
        //checking that if at all argument is provided
        if(argc>1){
        	
        	//print information as per the received arguments
		if(strcmp("-if",argv[1])==0){
			printf("\nProgram_File.c :: I am Fine\n");
		}
		else if(strcmp("-yf",argv[1])==0){
			printf("\nProgram_File.c :: Your are Fine\n");
		}
		else{
			printf("\nProgram_File.c :: No one is Fine\n");
		}
        	
        	
        }
        
        return 0;

}
