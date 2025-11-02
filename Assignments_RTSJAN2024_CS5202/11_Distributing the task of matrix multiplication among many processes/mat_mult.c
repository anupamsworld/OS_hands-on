#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>


// Function to compute product of a row in matrix A and column in matrix B
void computeRowColumn(int row, int col, int *matA, int *matB, int *matC, int cols_A, int cols_B) {
    int sum = 0;
    for (int i = 0; i < cols_A; i++) {
        sum += matA[row * cols_A + i] * matB[i * cols_B + col];
    }
    matC[row * cols_B + col] = sum;
}

int main() {
	int shmid, rows_A, cols_A, cols_B;
	int *matA, *matB, *matC;//these pointers are for pointing base addrsses of matrix A, B, C respectively

	// Read dimensions of matrix A
	printf("\nEnter the number of rows for matrix A: ");
	scanf("%d", &rows_A);
	printf("\nEnter the number of columns for matrix A: ");
	scanf("%d", &cols_A);
	
	printf("\nAs number of column of matrix A and number of rows of matrix B have to be same,");

	// Read dimensions of matrix B
	printf("\nEnter the number of columns for matrix B: ");
	scanf("%d", &cols_B);

	// Create shared memory for matrices A, B, and C
	shmid = shmget(IPC_PRIVATE, sizeof(int) * (rows_A * cols_A + cols_A * cols_B + rows_A * cols_B), 0666 | IPC_CREAT);
	if (shmid == -1) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}

	// Attach shared memory
	int *shared_memory = (int *)shmat(shmid, NULL, 0);
	if (shared_memory == (void *)-1) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}

	// Initializing the base pointers of matrices A, B, C respectively
	matA = shared_memory;
	matB = matA + rows_A * cols_A;
	matC = matB + cols_A * cols_B;

	// Read matrix A from user
	printf("Enter elements of matrix A (%d x %d) - row wise:\n", rows_A, cols_A);
	for (int i = 0; i < rows_A; i++) {
		for (int j = 0; j < cols_A; j++) {
			scanf("%d", &matA[i * cols_A + j]);
		}
	}

	// Read matrix B from user
	printf("Enter elements of matrix B (%d x %d) - row wise:\n", cols_A, cols_B);
	for (int i = 0; i < cols_A; i++) {
		for (int j = 0; j < cols_B; j++) {
			scanf("%d", &matB[i * cols_B + j]);
		}
	}

	
	// loop to create (rows_A*cols_B) number of child processes
	for (int i = 0; i < rows_A; i++) {
		pid_t pid = fork();
		if (pid == -1) {
		    perror("fork");
		    exit(EXIT_FAILURE);
		}
		else if (pid == 0) {// For child process
			for (int j = 0; j < cols_B; j++) {
			computeRowColumn(i, j, matA, matB, matC, cols_A, cols_B);
			}
			exit(EXIT_SUCCESS);
		}
	}

	// Wait for all child processes to finish
	for (int i = 0; i < rows_A; i++) {
		wait(NULL);
	}

	// Print resultant matrix C
	printf("Product matrix C (%d x %d):\n", rows_A, cols_B);
	for (int i = 0; i < rows_A; i++) {
		for (int j = 0; j < cols_B; j++) {
			printf("%d ", matC[i * cols_B + j]);
		}
		printf("\n");
	}

	// Detach and release shared memory
	if(shmdt(shared_memory)==-1){
		perror("Error in shmdt call: ");
		exit(EXIT_FAILURE);
	}
   	if(shmctl(shmid, IPC_RMID, NULL)){
   		perror("Error in shmctl call: ");
		exit(EXIT_FAILURE);
   	}

    return 0;
}
