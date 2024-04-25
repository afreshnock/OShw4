#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 4
#define MAXFILESIZE 1000000

double threadTime[NUM_THREADS] = {};

int threadInfo[NUM_THREADS][3] = {};

int *maxCharArray = NULL;

char **fileArray = NULL;

void getLineAndMax(int threadNumber, char *src_file, int startLine, int endLine) 
{
    clock_t start = clock();
    int maxCharArrayLocal[MAXFILESIZE];
    for (int i = startLine; i < endLine; i++) 
    {
        char *line = fileArray[i];
        size_t len = 0;
        int j = 0;
        char currentCharacter = line[j];
        char maxCharacter = line[j];
        while (currentCharacter != '\n')
        {
            if ((int)currentCharacter > (int)maxCharacter)
            {
                maxCharacter = currentCharacter;
            }
            j++;
            currentCharacter = line[j];
        }
        maxCharArray[i] = maxCharacter;
    }
    clock_t end = clock();
    double elapsedTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    threadTime[threadNumber] = elapsedTime;
}

int main() {
    clock_t start = clock();
    int extraLines = 0;
    size_t len = 0;
    char *line = NULL;
    //char *src_file = "/homes/dan/625/wiki_dump.txt";
    char *src_file = "names.txt";
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
    
    int totalLines = 0;
    fseek(file, 0, SEEK_SET);
    while (getline(&line, &len, file) != -1)
    {
        fileArray[totalLines] = strdup(line);
        totalLines++;
    }
    fclose(file);

    int linesPerThread = totalLines / NUM_THREADS;
    int remainder = totalLines % NUM_THREADS;
    int startLine = 0;
    int endLine = 0;

    maxCharArray = (int *)malloc(totalLines * sizeof(int));

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int tid = omp_get_thread_num();
        startLine = tid * linesPerThread;
        endLine = (tid + 1) * linesPerThread;
        getLineAndMax(tid, src_file, startLine, endLine);
    }

    clock_t end = clock();
    double elapsedTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("TOtal Time %lf\n", elapsedTime);
    printf("Average Lines Per Thread: %d\n", linesPerThread);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        printf("Thread: %d, Time: %lf\n", i, threadTime[i]);
    }
    
    //This is for the assignment
    for (int i = 0; i < totalLines;i++){
        printf("%d\n", maxCharArray[i]);
    }
    
    return 0;
}
