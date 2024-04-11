#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NUM_THREADS 4

#define ARRAY_SIZE 2000000
#define STRING_SIZE 16
#define ALPHABET_SIZE 26

pthread_mutex_t mutexsum;			// mutex for char_counts
FILE *file; 					// file
char char_array[ARRAY_SIZE][STRING_SIZE];
int char_counts[ALPHABET_SIZE];			// count of individual characters

char getRandomChar()
{
	int randNum = 0;
	char randChar = ' ';

	randNum = ALPHABET_SIZE * (rand() / (RAND_MAX + 1.0)); 	// pick number 0 < # < 25
	randNum = randNum + 97;				// scale to 'a'
	randChar = (char) randNum;

	// printf("%c", randChar);
	return randChar;
}

void init_arrays()
{
  int i, j; 

  pthread_mutex_init(&mutexsum, NULL);

  for ( i = 0; i < ARRAY_SIZE; i++) {
	for ( j = 0; j < STRING_SIZE; j++ ) {
		 char_array[i][j] = getRandomChar();
	}
  }

  for ( i = 0; i < ALPHABET_SIZE; i++ ) {
  	char_counts[i] = 0;
  }
}

void *count_array(void *myID)
{
  char theChar;
  int i, j, charLoc;
  int local_char_count[ALPHABET_SIZE];

  int startPos = ((int) myID) * (ARRAY_SIZE / NUM_THREADS);
  int endPos = startPos + (ARRAY_SIZE / NUM_THREADS);

  printf("myID = %d startPos = %d endPos = %d \n", (int) myID, startPos, endPos);

					// init local count array
  for ( i = 0; i < ALPHABET_SIZE; i++ ) {
  	local_char_count[i] = 0;
  }
					// count up our section of the global array
  for ( i = startPos; i < endPos; i++) {
	for ( j = 0; j < STRING_SIZE; j++ ) {
	         theChar = char_array[i][j];
		 charLoc = ((int) theChar) - 97;
		 local_char_count[charLoc]++;
	}
  }
					// sum up the partial counts into the global arrays
  pthread_mutex_lock (&mutexsum);
  for ( i = 0; i < ALPHABET_SIZE; i++ ) {
     char_counts[i] += local_char_count[i];
  }
  pthread_mutex_unlock (&mutexsum);

  pthread_exit(NULL);
}

void print_results()
{
  int i,j, total = 0;

  					// then print out the totals
  for ( i = 0; i < ALPHABET_SIZE; i++ ) {
     total += char_counts[i];
     printf(" %c %d\n", (char) (i + 97), char_counts[i]);
  }
  printf("\nTotal characters:  %d\n", total);
}

//This function takes a source file and desination array and copys the file info into the array
bool array_deserialize(const char *src_file, void *dst_data, const size_t elem_size, const size_t elem_count)
{
    if(src_file != NULL && dst_data != NULL && elem_size != 0 && elem_count != 0){//Checks arguments
        if(strchr(src_file, '\n') != NULL){//sscanf(src_file, "\n") == 0){I tried to use this but it does not work
	        return false;
	    }
    	FILE *file = fopen(src_file, "r");//trys to open file on read only
		if(file != NULL){//Checks to see if file is opened
			int num = fread(dst_data, elem_size, elem_count, file);//reads the file into data array
			fclose(file);//closes file
			if(num >= elem_count){//checks to see if the correct amount of elements have been read over
				printf("true");
				return true;//returns true if the correct amount of elements have been read over
			}
			printf("false");
    	}
    }
    return false;//returns false if any problems 
}

//This function given a line of characters returns the max ascii value from all the characters in the line
void getLineAndMax(void *threadNum){
	int lineNum = 0;
	char *line = NULL;
    size_t len = 0;
    ssize_t read;
	printf("Test");
	while ((read = getline(&line, &len, file)) != -1) {
		
		lineNum++;
		int i = 0;
		char currentCharacter = line[i];
		char maxCharacter = line[i];
		while(currentCharacter != '\n'){
			if((int)currentCharacter > (int)maxCharacter){
				maxCharacter = currentCharacter;
			}
			i++;
			currentCharacter = line[i];
		}
		printf("\nLine: %d", lineNum);
		printf("\nMax Char: %c", maxCharacter);
	}
}

main() {
	int i, rc;
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	void *status;


	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	char* src_file = "/homes/dan/625/wiki_dump.txt";
	//char* src_file = "/homes/brenth01/hw4/3way-pthread/test.txt";
	
	file = fopen(src_file, "r");
	if(file == NULL){
		printf("File did not open");
		exit(-1);
	}
	

	for (i = 0; i < NUM_THREADS; i++ ) {
		//rc = pthread_create(&threads[i], &attr, count_array, (void *)i);
		
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

	print_results();

	pthread_mutex_destroy(&mutexsum);
	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);

	
}

