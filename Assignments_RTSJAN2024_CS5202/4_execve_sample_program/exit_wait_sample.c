#include <stdio.h>
#include <errno.h>
#include <unistd.h> 

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
void printIntBits(int num) {
    int bit;
    // Loop through each bit position
    for (int i = sizeof(num) * 8 - 1; i >= 0; i--) {
        // Get the bit at position i using bitwise AND operation
        bit = (num >> i) & 1;
        // Print the bit
        printf("%d", bit);
        // Add space for better visualization
        if (i % 8 == 0)
            printf(" ");
    }
    printf("\n");
}
int main(int argc, char *argv[]) {
        
        
        
        
         int status2;
        pid_t pid=0;
        pid_t p1=0;
        printf("\nHello World!\n"); /* printing "Hello =World!\n" just like that */
        printf("\nsizeof(int)=%lu\n",sizeof(int));

        p1 = fork();
        if (p1 == 0) {
                /* this part is executed by child process */
                int i;

                /* the following part is just to keep the child executing something
                    so that it is live for some period and does not terminate */

                for (i=0; i<5; i++) {
                        printf("\n%d\n", i++);
                        getchar(); /* child process waits for user input, remains alive and does not terminate */
                }
                exit(5); /* the child process terminates with status 12, note that 12 is just a number,
                                 this status (12) is communicated to the parent process (in some form) and the process
                                 may use it (12) through wait() if it wishes to do so.
                              */

        }

       /* this part will be executed only  by the parent  process */
        pid = wait(&status2);
        printf("\npid = %d\nstatus2 = %d\nWEXITSTATUS(status2)=%d\n", pid, status2, WEXITSTATUS(status2)); /* Look the value of status that the  parent process prints! */
        
        printf("\n\n\n");
        printf("status2=\t\t\t");
        printIntBits(status2);
        
        printf("WEXITSTATUS(status2)=\t\t");
        printIntBits(5);
        
        /*
        The child process exit with status 5. It sets the value in most significant byte of 2 byte space. Least significant byte is reserved by OS.
        Hence, 5(00000000 00000000 00000000 00000101) is transformed into 1280(00000000 00000000 00000101 00000000)
        */

}
