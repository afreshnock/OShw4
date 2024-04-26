#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_THREADS 8
#define MAXFILESIZE 1000001


pthread_mutex_t mutex;		// mutex for theadTime

double threadTime[NUM_THREADS + 1] = {};

int threadInfo[NUM_THREADS + 1][3] = {};

char **fileArray = NULL;

int *maxCharArray = NULL;

char *src_file = "/homes/dan/625/wiki_dump.txt";
//char *src_file = "names.txt";

void *getLineAndMax(void *myID)
{
    int threadNumber = (int)myID;
    
    int startLine = threadInfo[threadNumber][0];
    int endLine = threadInfo[threadNumber][1];
    int *maxCharArrayLocal = (int *)malloc((endLine - startLine) * sizeof(int));
    char *line = NULL;

    clock_t start = clock();

    for(int i = startLine; i < endLine;i++)
    {
        line = fileArray[i];
        char currentCharacter = '\0';
        char maxCharacter = '\0';
        for(int j = 0; currentCharacter != '\n' ; j++)
        {
            currentCharacter = line[j];
            if ((int)currentCharacter > (int)maxCharacter)
            {
                maxCharacter = currentCharacter;
            }
            
        }
        maxCharArrayLocal[i - startLine] = (int)maxCharacter;
    }
    
    //pthread_mutex_lock(&mutex);
    for(int i = startLine; i < endLine; i++)
    {   
        maxCharArray[i] =  maxCharArrayLocal[i - startLine];
    }
    //pthread_mutex_unlock(&mutex);

    clock_t end = clock();
    double elapsedTime = ((double)(end - start)) / CLOCKS_PER_SEC;

    //pthread_mutex_lock(&mutex);
    threadTime[threadNumber] = elapsedTime;
    //pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main() {
	int rc;
	pthread_t threads[NUM_THREADS + 1];
	pthread_attr_t attr;
	//int status;
	char *line = NULL;
	size_t len = 0;
	int totalAmountOfLines = 0;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	//pthread_mutex_init(&mutex, NULL);

    FILE *file = fopen(src_file, "r");
    if (file == NULL)
    {
        printf("File did not open");
        exit(-1);
    }
    
    fileArray = (char **)malloc(MAXFILESIZE * sizeof(char *));
    
    if (fileArray == NULL) {
        printf("Memory allocation failed\n");
        exit(-1);
    }
    
    int i = 0;
    while (getline(&line, &len, file) != -1)
    {
        fileArray[i] = line;
        i++;
    }
    totalAmountOfLines = i;
    
    fclose(file);
    
    maxCharArray = (int *)malloc(fileSize * sizeof(int));
    
    int linesPerThread = totalAmountOfLines / NUM_THREADS;
    int remainder = totalAmountOfLines % NUM_THREADS;
    int startLine = 0;
    clock_t start = clock();
	for (int i = 1; i <= NUM_THREADS; i++ ) {
        int endLine = startLine + linesPerThread - 1;
        if (i < remainder)
        {
            endLine++;
        }
    
        threadInfo[i][0] = startLine;
        threadInfo[i][1] = endLine;
        threadInfo[i][2] = i;

        rc = pthread_create(&threads[i], &attr, getLineAndMax, (void *)i);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }  
        startLine = endLine + 1;
	}
    
	/* Free attribute and wait for the other threads */
	
	for(int i=1; i<=NUM_THREADS; i++) {
	     rc = pthread_join(threads[i], NULL);
	     if (rc) {
		   printf("ERROR; return code from pthread_join() is %d\n", rc);
		   exit(-1);
	     }
	}
    pthread_attr_destroy(&attr);
    clock_t end = clock();
    double elapsedTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Total Time %lf\n", elapsedTime);
	printf("Average Lines Per Thread: %d\n", linesPerThread);
    for (int i = 1; i <= NUM_THREADS; i++)
    {
        printf("Thread: %d, Time: %lf\n", i, threadTime[i]);
    }
    /*
    for (int i = 1; i <= NUM_THREADS; i++)
    {
        printf("Thread: %d, start: %d\n", i, threadInfo[i][0]);
        printf("Thread: %d, end: %d\n", i, threadInfo[i][1]);
    }
    */
    
    //This is for the assignment
    for (int i = 0; i < totalAmountOfLines;i++){
        printf("%d\n", maxCharArray[i]);
    }
    
    printf("\n");
	printf("Main: program completed. Exiting.\n");
	pthread_mutex_destroy(&mutex);
	free(maxCharArray);
	free(fileArray);
	pthread_exit(NULL);
}

