/* Including Libraries */
#include <stdio.h>
#include <pthread.h>
#include "testmycall.h"

/* Headers */
void *getStats(void *p);
void *openProgram(void *p);

/*
 * Global variable (and corresponding mutex) to keep track of how many instances
 * of the program are running
*/
pthread_mutex_t count_mutex;
int count;

/*
 * Main: Asks the user for the number of instances of the program to run and for
 * the time in between each call. Validates input. Creates a thread that runs
 * getStats and then creates the specified number of threads each running an
 * instance of the program (openProgram).
*/
int main (int argc, char *argv[]) {
	int i, n, s;
	count = 0;

	if (argc != 3) {
		fprintf(stderr, "usage: ./testmycall <n_instances> <n_seconds>\n");
		return -1;
	}
	if ((n = atoi(argv[1])) < 1) {
		fprintf(stderr, "%s must be >= 0\n", argv[1]);
		return -1;
	}
	if ((s = atoi(argv[2])) < 1) {
		fprintf(stderr, "%s must be >= 0\n", argv[2]);
		return -1;
	}

	pthread_mutex_init(&count_mutex, NULL);

	pthread_t tid[n + 1];

	pthread_create(&tid[0], NULL, getStats, (void *)n);

	for (i = 1; i <= n; i++) {
		sleep(s);
		pthread_create(&tid[i], NULL, openProgram, NULL);
	}
	for (i = 0; i <= n; i++)
		pthread_join(tid[i], NULL);
	return 0;
}

/* Function that runs in separate thread calling syscalls every second */
void *getStats(void *p) {
	int i = 1, n = (int)p;
	while (i) {
		pthread_mutex_lock(&count_mutex);
		// Kill all threads if number of instances has been reached
		if (count == n) {
			system("pkill -9 -f mahjongg");
			pthread_exit(0);
		}
		pthread_mutex_unlock(&count_mutex);
		// Call syscalls and output stats
		printf("Call %d\n", i);
		printf("Claimed > %ld\n", syscall(__NR_claimed));
		printf("Free    > %ld\n\n", syscall(__NR_free));
		sleep(1);
		i++;
	}
}

/* Function that runs in separate thread and opens an instance of a program */
void *openProgram(void *p) {
	pthread_mutex_lock(&count_mutex);
	count++;
	pthread_mutex_unlock(&count_mutex);
	system("/usr/games/mahjongg");
	pthread_exit(0);
}
