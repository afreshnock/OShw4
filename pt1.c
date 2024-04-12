#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define NUM_THREADS 8						// Number of threads. Can be changed for scale ability

pthread_mutex_t mutex;						// mutex for reading the file

double threadTime[NUM_THREADS] = {1,1,1,1,1,1,1,1};

int threadInfo[NUM_THREADS][3] = {};


//This function given a line of characters returns the max ascii value from all the characters in the line
void *getLineAndMax(void *threadIndex){
	
	/* Trys to open file */
	char* src_file = "/homes/dan/625/wiki_dump.txt";
	//char* src_file = "/homes/brenth01/hw4/3way-pthread/test.txt"; // Test
	FILE *file  = fopen(src_file, "r");
	if (file == NULL) {
		printf("File did not open");
		exit(-1);
	}
	clock_t start, end;
	
	int threadNum = (int)threadIndex;
    int startLine = threadInfo[threadNum][0];
    int endLine = threadInfo[threadNum][1];
	int lineNum = 0;
	char *line = NULL;
    size_t len = 0;
    ssize_t read;

	start = clock();
	fseek(file, startLine, SEEK_SET);

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
	}
	free(line);
	end = clock();
	threadTime[threadNum] = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("Thead Number %d ", threadNum);
	printf("Thread Time %f \n", threadTime[threadNum]);
	fclose(file);
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
	FILE *file  = fopen(src_file, "r");
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
	fclose(file);
	/* Calculates amount of lines that a single tread runs */
	int linesPerThread = totalLines / NUM_THREADS;
    int remainder = totalLines % NUM_THREADS;
    int startLine = 1;
	
	

	/* Makes theads and runs them with the given amount of lines */
	for (i = 0; i < NUM_THREADS; i++ ) {
		int endLine = startLine + linesPerThread - 1;
		if (i < remainder) {
            endLine++;
        }
		//pthread_mutex_lock(&mutex);
		threadInfo[i][0] = startLine;
		threadInfo[i][1] = endLine;
		threadInfo[i][2] = i;
        startLine = endLine + 1;
		
		rc = pthread_create(&threads[i], &attr, getLineAndMax, (void *)i); 
		//pthread_mutex_unlock(&mutex);
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

	/* Print results */
	printf("Average Lines Per Thread: %d\n", linesPerThread);
	for (i = 0; i < NUM_THREADS; i++) {
		printf("Thread: %d, Time: %lf\n", i, threadTime[i]);
	}
	
	pthread_mutex_destroy(&mutex);
	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);

	
}

