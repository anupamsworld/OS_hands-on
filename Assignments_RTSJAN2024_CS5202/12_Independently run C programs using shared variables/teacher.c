#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>

#define MAX_STUDENTS 1000
int n;

struct att {
	int roll;
	time_t tm;
};

  struct att *rolls;
int shmid;

void sigint_handler(int sig) {

  for (int i = 0; i < n; i++) {
            printf("Student with roll number %d marked attendance at %s", rolls[i].roll, ctime(&rolls[i].tm));
  }
  printf("\nTerminating teacher process...\n");

  // Detach and remove shared memory
  shmctl(shmid, IPC_RMID, NULL);

  exit(0);
}

int main(int argc, char *argv[]) {

	n = atoi(argv[1]);
	if (argc != 2) {
		printf("Usage: %s <number_of_students>\n", argv[0]);
		exit(1);
	}

	if (n <= 0 || n > MAX_STUDENTS) {
		printf("Invalid number of students. Please provide a positive integer less than or equal to %d.\n", MAX_STUDENTS);
		exit(1);
	}


	// Generate a key using ftok()
	key_t key = ftok("teacher.c", 'A');
	if (key == -1) {
		perror("ftok");
		exit(1);
	}

	// Create shared memory segment
	shmid = shmget(key, n * sizeof(struct att), IPC_CREAT | 0666);
	if (shmid < 0) {
	perror("shmget");
		exit(1);
	}

	// Attach shared memory segment
	rolls = (struct att *)shmat(shmid, NULL, 0);
	if (rolls == (struct att *)(-1)) {
		perror("shmat");
		exit(1);
	}

	// Initialize shared array
	for (int i = 0; i < n; i++) {
		rolls[i].roll = -1;
	}

	// Register SIGINT handler
	signal(SIGINT, sigint_handler);

	printf("Teacher process initiated. Press Ctrl+C to terminate.\n");

	while (1) {
		// Do nothing, wait for Ctrl+C
	}

	return 0;
}

