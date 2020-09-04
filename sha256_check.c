#include <stdio.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define THREADS 128L
#define BATCH 78125000L // 1 0000 000 000 divided by 128
#define TOTALNUM 10000000000

struct hashPair {
	long long int number;
	unsigned char hash[32]; 
};


struct hashPair hashes[TOTALNUM];


static void printHash(unsigned char * hash) {
	for (int i = 0; i < 32; ++i)
		printf("%02X", *(hash + i));
	putchar(' ');
}


static int compareHash(const void * x, const void * y) {
	uint64_t * a, * b;
        a = ((struct hashPair *) x)->hash;
	b = ((struct hashPair *) y)->hash;
	int result = 0;
	for (int i = 0; i < 4; ++i) {
		if(a[i] > b[i]) {
			result = 1;
			break;
		} else if (a[i] < b[i]) {
			result = -1;
			break;
		}
	}
	return result;
}


static void * fillHash(void * threadId) {
	unsigned char * number;
	number = malloc(11);
	for (long long int i = BATCH * (long long int) threadId; i < (BATCH * (1 + (long long int)threadId)); ++i) {
		hashes[i].number = i;
		snprintf(number, 11, "%010lld", i);
		SHA256(number, 10, hashes[i].hash);
	}
	free(number);
	qsort(&hashes[BATCH * (long long int) threadId], BATCH, sizeof(struct hashPair), compareHash);
	return (void *) (intptr_t) 0;
}


static void * checkEquals(void * threadId) {
        for (long long int i = BATCH * (long long int) threadId + 1; i < BATCH * (1 + (long long int) threadId); ++i) {
                if(!compareHash((void *) &hashes[i], (void *) &hashes[i - 1])) {
                        printf("number %lld and %lld give the same hash:\n", hashes[i].number, hashes[i-1].number);
                        printHash(hashes[i].hash);
                        printHash(hashes[i-1].hash);
                        putchar('\n');
                        fflush(stdout);
                }
        }
	return (void *) (intptr_t) 0;
}


int main() {
	time_t t0 = time(0);
	pthread_t * myThreads;
	myThreads = malloc(THREADS * sizeof(pthread_t));
	for (int i = 0; i < THREADS; ++i)
		pthread_create(&myThreads[i], NULL, fillHash, (void *) (intptr_t) i);
	for (int i = 0; i < THREADS; ++i)
		pthread_join(myThreads[i], NULL);
	time_t t1 = time(0);
        int diff = t1 - t0;
        printf("\nI started sorting at: %02d:%02d\n\n", diff / 60, diff % 60);
	qsort(hashes, TOTALNUM, sizeof(struct hashPair), compareHash);
	t1 = time(0);
        diff = t1 - t0;
        printf("\nI finished sorting at: %02d:%02d\n\n", diff / 60, diff % 60);
	for (int i = 0; i < THREADS; ++i)
                pthread_create(&myThreads[i], NULL, checkEquals, (void *) (intptr_t) i);
        for (int i = 0; i < THREADS; ++i)
                pthread_join(myThreads[i], NULL);
	for (long long int i = 1; i < THREADS; ++i) {
		if(!compareHash((void *) &hashes[i * BATCH], (void *) &hashes[i * BATCH - 1])) {
			printf("number %lld and %lld give the same hash:\n", hashes[i].number, hashes[i-1].number);
			printHash(hashes[i].hash);
			printHash(hashes[i-1].hash);
			putchar('\n');
			fflush(stdout);
		}
	}
	free(myThreads);
	t1 = time(0);
	diff = t1 - t0;
        printf("\nIt took: %02d:%02d\n\n", diff / 60, diff % 60);
	// If somebody needs to print or save certain hashes
	/* for (long long int i = 0; i < TOTALNUM; ++i) {
		unsigned char * number;
	        number = malloc(11);
                snprintf(number, 11, "%010lld", hashes[i].number);
		fprintf(stdout, "Number %s:\n", number);
		free(number);
		printHash(hashes[i].hash);
		putc('\n', stdout);
	} */
	return 0;
}
