#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define NUM_THREADS 4

#define ARRAY_SIZE 2000000
#define STRING_SIZE 16
#define ALPHABET_SIZE 26

pthread_mutex_t mutexsum;			// mutex for char_counts
FILE *file; 					// file
char char_array[ARRAY_SIZE][STRING_SIZE];
int char_counts[ALPHABET_SIZE];			// count of individual characters

//This function given a line of characters returns the max ascii value from all the characters in the line
void *getLineAndMax(void *threadNum){
	int *threadRange = (int *)threadNum;
    int startLine = threadRange[0];
    int endLine = threadRange[1];
	int lineNum = 0;
	char *line = NULL;
    size_t len = 0;
    ssize_t read;

	pthread_mutex_lock(&mutexsum);
	fseek(file, 0, SEEK_SET);
    for (int i = 0; i < startLine; i++) {
        if (getline(&line, &len, file) == -1) {
            printf("Unable to seek to the starting line\n");
            return;
        }
    }

	while ((read = getline(&line, &len, file)) != -1 && lineNum < endLine) {
		lineNum++;
		int i = 0;
		char currentCharacter = line[i];
		char maxCharacter = line[i];
		while (currentCharacter != '\n') {
			if ((int)currentCharacter > (int)maxCharacter) {
				maxCharacter = currentCharacter;
			}
			i++;
			currentCharacter = line[i];
		}
		
		//printf("Thead Line Range: %d - %d  ", startLine, endLine);
		//printf("Line: %d ", lineNum);
		//printf("Max Char: %c\n", maxCharacter);
		
	}
	pthread_mutex_unlock(&mutexsum);
	free(line);
}

int main() {
	int i, rc;
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	void *status;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	/* Trys to open file */
	char* src_file = "/homes/dan/625/wiki_dump.txt";
	//char* src_file = "/homes/brenth01/hw4/3way-pthread/test.txt"; // Test
	file = fopen(src_file, "r");
	if (file == NULL) {
		printf("File did not open");
		exit(-1);
	}
	
	/* Calculate the total number of lines in the file */
    int totalLines = 0;
	fseek(file, 0, SEEK_SET);
    while ((read = getline(&line, &len, file)) != -1) {
        totalLines++;
    }

	/* Calculates amount of lines that a single tread runs */
	int linesPerThread = totalLines / NUM_THREADS;
    int remainder = totalLines % NUM_THREADS;
    int startLine = 1;
	double threadTime[4] = {};
	clock_t start, end;
	/* Makes theads and runs them with the given amount of lines */
	for (i = 0; i < NUM_THREADS; i++ ) {
		int endLine = startLine + linesPerThread - 1;
		if (i < remainder) {
            endLine++;
        }
		int threadRange[2] = {startLine, endLine};
        startLine = endLine + 1;
		start = clock();
		rc = pthread_create(&threads[i], &attr, getLineAndMax, (void *)threadRange); 
		end = clock();
		threadTime[i] = ((double)(end - start)) / CLOCKS_PER_SEC;
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	/* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);
	for (i=0; i<NUM_THREADS; i++) {
		rc = pthread_join(threads[i], &status);
		if (rc) {
		printf("ERROR; return code from pthread_join() is %d\n", rc);
		exit(-1);
		}
	}

	for (i = 0; i < NUM_THREADS; i++) {
		printf("Thread: %d, Time: %lf\n", i, threadTime[i]);
	}
	fclose(file);
	pthread_mutex_destroy(&mutexsum);
	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);

	
}

