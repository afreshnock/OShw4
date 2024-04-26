#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define MAX_NAME_LENGTH 1000
#define NUM_LINES 1000000

int main() {
    FILE *file;
    char name[MAX_NAME_LENGTH];
    char maxCharArray[NUM_LINES];
    
    file = fopen("/homes/dan/625/wiki_dump.txt", "r");
    if (file == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }
    clock_t start = omp_get_wtime();
    
    #pragma omp parallel
    {
        char maxCharacter;
        #pragma omp for schedule(dynamic, 1)
        for (int i = 0; i < NUM_LINES; i++) {
            fgets(name, MAX_NAME_LENGTH, file);
            maxCharacter = name[0];
            for (int j = 1; name[j] != '\0'; j++) {
                if (name[j] > maxCharacter) {
                    maxCharacter = name[j];
                }
            }
            maxCharArray[i] = maxCharacter;
        }
        double end = omp_get_wtime(); 
        printf("Thread: %d time: %lf \n", omp_get_thread_num(), end - start);
    }
    fclose(file);
    clock_t end = clock();
    double elapsedTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Total Time %lf\n", elapsedTime);

    for (int i = 0; i < NUM_LINES; i++) {
        printf("%d ", (int)maxCharArray[i]);
    }
    printf("\n");
    
    return 0;
}
