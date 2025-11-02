#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define MAX_STUDENTS 1000

struct att{
    int roll;
    time_t tm;
};

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <roll_number>\n", argv[0]);
		exit(1);
	}

	int roll = atoi(argv[1]);
	if (roll <= 0 || roll > MAX_STUDENTS) {
		printf("Invalid roll number. Please provide a positive integer less than or equal to %d.\n", MAX_STUDENTS);
		exit(1);
	}

	int shmid;
	struct att *rolls;

	// Generate a key using ftok()
	key_t key = ftok("teacher.c", 'A');
	if (key == -1) {
		perror("ftok");
		exit(1);
	}

	// Get shared memory segment
	shmid = shmget(key, 0, 0);
	if (shmid < 0) {
		perror("shmget");
		exit(1);
	}

	// Attach to existing shared memory segment
	rolls = (struct att *)shmat(shmid, NULL, 0);
	if (rolls == (struct att *)(-1)) {
		perror("shmat");
		exit(1);
	}

	// Find an empty slot and mark attendance
	if (rolls[roll-1].roll == -1) {
		rolls[roll-1].roll = roll;
		rolls[roll-1].tm = time(NULL);
		printf("Student with roll number %d marked attendance at %s", roll, ctime(&rolls[roll].tm));
	}

	// Detach from shared memory segment
	shmdt(rolls);

	return 0;
}

