#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include<sys/wait.h>



int main(int argc, char *argv[]){
	
	
	/*******START OF MATRIX INPUT*******/
	
	int i, j, m1, n1, m2, n2;
		
	printf("\nWarning: Number of columns in 1st matrix mast be equal to the number of rows in 2nd matrix.\n");
	printf("\nWarning: Any element of resultant matrix could be between 0 and 255(inclusive).\n");

	printf("\nInput for matrix 1\n");
	printf("Enter number of rows : ");
	scanf("%d", &m1);
	printf("Enter number of columns : ");
	scanf("%d", &n1);
	
	int matrix_1[m1][n1];

	/* Input data in matrix_1 */
	for (i = 0; i < m1; i++){
		for (j = 0; j < n1; j++){
		    printf("Enter data in [%d][%d]: ", i, j);
		    scanf("%d", &matrix_1[i][j]);
		}
	}
	
	printf("\nInput for matrix 2\n");
	printf("Enter number of rows : ");
	scanf("%d", &m2);
	printf("Enter number of columns : ");
	scanf("%d", &n2);
	
	if(n1!=m2){
		printf("\nCondition is violated. <Number of columns in 1st matrix mast be equal to the number of rows in 2nd matrix.>\nAborting\n");
		exit(1);
	}
	
	int matrix_2[m2][n2];
	
	
	/* Input data in matrix_2 */
	for (i = 0; i < m2; i++){
		for (j = 0; j < n2; j++){
		    printf("Enter data in [%d][%d]: ", i, j);
		    scanf("%d", &matrix_2[i][j]);
		}
	}

	printf("\nCaptured Matrix 1:\n");
	/* Display the matrix_1 */
	for (i = 0; i < m1; i++){
		for (j = 0; j < n1; j++){
		    printf("%d\t", matrix_1[i][j]);
		}
		printf("\n");
	}
	
	printf("\nCaptured Matrix 2:\n");
	/* Display the matrix_2 */
	for (i = 0; i < m2; i++){
		for (j = 0; j < n2; j++){
		    printf("%d\t", matrix_2[i][j]);
		}
		printf("\n");
	}
	/*******END OF MATRIX INPUT*******/
	
	
	pid_t pid;
	int status;
	
	
	
	int matrix_result[m1][n2];
	
	for(int x=0;x<m1;x++){
		for(int y=0;y<n2;y++){
			pid=fork();
			if(pid==0){
				int sum=0;
				//for(int p=0;p<n1;p++){
					for(int q=0;q<n1;q++){
						sum += matrix_1[x][q] * matrix_2[q][y] ;
					}
				//}
				printf("\nsum=%d\n",sum);
				exit(sum);
			}
			else if(pid>0){
				int wait_status=waitpid(pid, &status, 0); //waiting for the child process Pi to complete its work.
				if( WIFEXITED(status)){
					printf("\nChild process P[%d][%d](pid=%d) is done with executing. Now in parent.\n",x,y,wait_status);
					matrix_result[x][y] = WEXITSTATUS(status) ;
				}
				else{
					printf("\nChild process has not terminated normally.\n");
					exit(1);
				}
				
			}
		}
	}
	
	printf("\nResultant matrix:\n");
	/* Display the matrix_2 */
	for (i = 0; i < m1; i++){
		for (j = 0; j < n2; j++){
		    printf("%d\t", matrix_result[i][j]);
		}
		printf("\n");
	}
	
	
	printf("\n");
	return 0;
}

/*
Sample command to run cpedp.c= ./a.out ./pf -if /bin/ls -l ./pf2 -machine
pf. pf2 are executable files which are created by compiling program_file.c and program_file_2.c repectively
Apart from own program, this program accepts executable files of system also. For example "ls"
*/

