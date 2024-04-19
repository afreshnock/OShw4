#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_THREADS 4

pthread_mutex_t mutex;		// mutex for theadTime

double threadTime[NUM_THREADS] = {};

int threadInfo[NUM_THREADS][3] = {};

int maxCharArray[1000000] = {};

char *src_file = "/homes/dan/625/wiki_dump.txt";
//char *src_file = "names.txt";

void *getLineAndMax(void *myID)
{
    int threadNumber = (int*)myID;
    printf("Thread: %d", threadNumber);

    FILE *file = fopen(src_file, "r");
    if (file == NULL)
    {
        printf("File did not open");
        exit(-1);
    }
    int startLine = threadInfo[threadNumber][0];
    int endLine = threadInfo[threadNumber][1];

    fseek(file, startLine, SEEK_SET); // need rework

    int lineNum = startLine;
    char *line[1000] = {};
    size_t len = 0;
    //ssize_t read;

    clock_t start = clock();

    

    while ( lineNum < endLine)
    {
        
        pthread_mutex_lock(&mutex);
        for(int i = 0; i < 1000; i++){ 
            if(getline(&line[i], &len, file) == -1){
                printf("End of File");
                break;
            }
        } 
        pthread_mutex_unlock(&mutex);
        for(int j =0; j <1000;j++){

            
            int i = 0;
            char currentCharacter = line[j][i];
            char maxCharacter = line[j][i];
            while (currentCharacter != '\n')
            {
                if ((int)currentCharacter > (int)maxCharacter)
                {
                    maxCharacter = currentCharacter;
                }
                i++;
                currentCharacter = line[j][i];
            }    
            maxCharArray[lineNum] = maxCharacter;
            //free(line[j]);
        }
        lineNum+= 1000;
    }
    
    fclose(file);

    clock_t end = clock();
    double elapsedTime = ((double)(end - start)) / CLOCKS_PER_SEC;

   // pthread_mutex_lock(&mutex);
    threadTime[threadNumber] = elapsedTime;
    //pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
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

	pthread_mutex_init(&mutex, NULL);

    FILE *file = fopen(src_file, "r");
    if (file == NULL)
    {
        printf("File did not open");
        exit(-1);
    }

    int totalLines = 0;
    fseek(file, 0, SEEK_SET);
    while ((read = getline(&line, &len, file)) != -1)
    {
        totalLines++;
    }
    fclose(file);

    int linesPerThread = totalLines / NUM_THREADS;

	for (i = 0; i < NUM_THREADS; i++ ) {
        int endLine, startLine;

        startLine = i * linesPerThread; 
        endLine = (i+1) * linesPerThread;

        threadInfo[i][0] = startLine;
        threadInfo[i][1] = endLine;
        threadInfo[i][2] = i;

	    rc = pthread_create(&threads[i], &attr, getLineAndMax, (void *)i);
	    if (rc) {
	        printf("ERROR; return code from pthread_create() is %d\n", rc);
		    exit(-1);
	    }
	}

	/* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);
	for(i=0; i<NUM_THREADS; i++) {
	     rc = pthread_join(threads[i], &status);
	     if (rc) {
		   printf("ERROR; return code from pthread_join() is %d\n", rc);
		   exit(-1);
	     }
	}

	printf("Average Lines Per Thread: %d\n", linesPerThread);
    for (i = 0; i < NUM_THREADS; i++)
    {
        printf("Thread: %d, Time: %lf\n", i, threadTime[i]);
    }

    /* This is for the assignment
    for (i = 0; i < sizeof(maxCharArray);i++){
        printf("%d", maxCharArray[i]);
    }
    */
	pthread_mutex_destroy(&mutex);
	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);
}

