#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define NUM_THREADS 4
int NUM_THREADS;

#define ARRAY_SIZE 2000000
#define STRING_SIZE 16
#define ALPHABET_SIZE 26

char char_array[ARRAY_SIZE][STRING_SIZE];
int char_counts[ALPHABET_SIZE];			// global count of individual characters
int local_char_count[ALPHABET_SIZE];

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

  printf("Initializing arrays.\n"); fflush(stdout);

  for ( i = 0; i < ARRAY_SIZE; i++) {
	for ( j = 0; j < STRING_SIZE; j++ ) {
		 char_array[i][j] = getRandomChar();
	}
  }

  for ( i = 0; i < ALPHABET_SIZE; i++ ) {
  	char_counts[i] = 0;
  }
}

void *count_array(void *rank)
{
  char theChar;
  int i, j, charLoc;
  int myID =  *((int*) rank);

  int startPos = ((long) myID) * (ARRAY_SIZE / NUM_THREADS);
  int endPos = startPos + (ARRAY_SIZE / NUM_THREADS);

  printf("myID = %d startPos = %d endPos = %d \n", myID, startPos, endPos); fflush(stdout);

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
}

void print_results()
{
  int i,j, total = 0;

  					// then print out the totals
  for ( i = 0; i < ALPHABET_SIZE; i++ ) {
     total += char_counts[i];
     printf(" %c %d\n", (char) (i + 97), char_counts[i]);
  }
  printf("\nTotal characters:  %d\n", total); fflush(stdout);
}

void get_max(char *buffer, int buffer_size, int rank) {
    int maxVal = 0;
    char maxChar = 0;
    int i = 0;
    char currentChar;

    //I think this is right. uses a built in time function to get the time it takes for the process to run
    double start, end;
    start = MPI_Wtime();

    char line = strtok(buffer, "\n");
	//inspiration taken from get_line_and_max
	int line_num = 0;
    while (line != NULL) {
		int i = 0;
        maxChar = line[0];
		char currentCharacter = line[i];
		char maxCharacter = line[i];
		while (currentCharacter != '\n') {
			if ((int)currentCharacter > (int)maxCharacter) {
				maxCharacter = currentCharacter;
			}
			i++;
			currentCharacter = line[i];
		}
		line = strtok(NULL, "\n");
    }

    end = MPI_Wtime();

    printf("Rank %d processing time: %f seconds\n", rank, end - start);
}

void split_file(int rank, int num_processes){
	MPI_File m_file;
    MPI_Status status;
	MPI_Offset filesize;
    char *buffer;
    int buffer_size;

    //opens the file
    MPI_File_open(MPI_COMM_WORLD, "/homes/dan/625/wiki_dump.txt", MPI_MODE_RDONLY, MPI_INFO_NULL, &m_file);//

	//caluclates the size of the file
	rc = MPI_File_get_size(m_file, &filesize);
	if (rc != MPI_SUCCESS) {
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

    //gets the buffer_size based on the size of the file and the number of mpi processes
    buffer_size = filesize / num_processes;
    buffer = (char *) malloc(buffer_size);

    //splits the file and stores the segment in m_file
    MPI_File_set_view(m_file, rank * buffer_size, MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);

    //reads the segment of the file and stores it in the buffer
    MPI_File_read(m_file, buffer, buffer_size, MPI_CHAR, &status);

    //gets the max ascii value per line
	get_max(buffer*, buffer_size, rank);

    free(buffer);
    MPI_File_close(&m_file);
}

main(int argc, char* argv[]) 
{
	int i, rc;
	int numtasks, rank;
	MPI_Status Status;
	

	rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
	  printf ("Error starting MPI program. Terminating.\n");
          MPI_Abort(MPI_COMM_WORLD, rc);
        }

        MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	NUM_THREADS = numtasks;
	printf("size = %d rank = %d\n", numtasks, rank);
	fflush(stdout);

	if ( rank == 0 ) {
		init_arrays();
	}
	MPI_Bcast(char_array, ARRAY_SIZE * STRING_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
		
	//count_array(&rank);
	split_file(rank, numtasks)

	MPI_Reduce(local_char_count, char_counts, ALPHABET_SIZE, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if ( rank == 0 ) {
		print_results();
	}

	MPI_Finalize();
	return 0;
}

