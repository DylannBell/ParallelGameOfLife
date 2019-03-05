#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include<sys/time.h>
#include <math.h>

// Compile using gcc -fopenmp gol.c

// Quick access to grid elements.
#define BOARD(G, X, Y) ((G)[BOARD_DIM*(X)+(Y)]) 

// Modulus function that works for negative numbers.
#define mod(a,b) ((((a)%(b))+(b))%(b)) 

// Global variables that can be updated for timing purposes.
int TURNS = 100;
int BOARD_DIM = 1024;
int NUM_THREADS = 4;

// Print grid to stdout 
void print_grid(int **grid) {
	for(int r = 0; r < BOARD_DIM; r++) {
		for(int c = 0; c < BOARD_DIM; c++) {
			printf(" %d ", grid[r][c]);
		} 
		printf("\n");
	} 
	printf("\n");
}

// Free memory of the array
void freeArray(int **grid) {
	for (int i = 0; i < BOARD_DIM; i++) {
		free(grid[i]);
	}
	free(grid);
}



void writeGridToFile(int **grid, int turn) {

	// Name the file that includes size and turn number.
	char file_name[100] = {'\0'};
	sprintf(file_name, "grid%d-%d.txt", turn+1, BOARD_DIM);

	FILE *f = fopen(file_name, "w");
	if (f == NULL) {
	    printf("Error opening file!\n");
	    exit(EXIT_FAILURE);
	}

	// Write the grid to the file.
	for(int r = 0; r < BOARD_DIM; r++) {
		for(int c = 0; c < BOARD_DIM; c++) {
			fprintf(f, "%d ", grid[r][c]);
		} 
		fprintf(f, "\n");
	} 
	fprintf(f, "\n");
	fclose(f);
}


//Standard pragma for implementation.
void parallel1() {

	// Seed the random number.
	srand(time(NULL));

	// Creating the boards in the heap and initialising to a random binary number.
	int **board = (int**)malloc(BOARD_DIM * sizeof(int *));
	int **board_new = (int**)malloc(BOARD_DIM * sizeof(int *));

	for(int i = 0; i < BOARD_DIM; i++) {
		board[i] = (int *)malloc(BOARD_DIM * sizeof(int));
		board_new[i] = (int *)malloc(BOARD_DIM * sizeof(int));

		for(int j = 0; j < BOARD_DIM; j++) {
			int random_number = rand() % 2;
			board[i][j] = random_number;
			board_new[i][j] = random_number;
		}
	}

	// Looping through each turn.
	for (int turn = 0; turn < TURNS; turn++) {

		// Print grid
		//print_grid(board);

		// Parralelise code using collapse and specify num_threads
		#pragma omp parallel for num_threads(NUM_THREADS)
		for (int i = 0; i < BOARD_DIM; i++) {
			for (int j = 0; j < BOARD_DIM; j++) {

				// Set up coordinates for counting neighbours.
				int iUp = mod (i-1, BOARD_DIM);
				int iDown = mod (i+1, BOARD_DIM);
			    int jLeft = mod (j-1, BOARD_DIM);
			    int jRight = mod (j+1, BOARD_DIM);

			    // Count neighbours for current cell.
				int neighbours = 
				board[iUp][j] + board[iDown][j] + 
				board[i][jLeft] + board[i][jRight];

				// Perform game of life operations on current cell.
				if (board[i][j] == 1 && neighbours < 2) {
					board_new[i][j] = 0;
				}
				if (board[i][j] == 1 && (neighbours == 2 || neighbours == 3)) {
					board_new[i][j] = 1;
				}
				if (board[i][j] == 1 && neighbours > 3) {
					board_new[i][j] = 0;
				}
				if (board[i][j] == 0 && neighbours == 3) {
					board_new[i][j] = 1;
				}
			}
		}

		//Copy the new board onto current board
		for (int x = 0; x < BOARD_DIM; x++) {
			for (int y = 0; y < BOARD_DIM; y++) {
				board[x][y] = board_new[x][y];
			}
		}
	}
	freeArray(board);
	freeArray(board_new);

}

// Pragma for implementation with collapse(2).
void parallel2() {

	// Seed the random number.
	srand(time(NULL));

	// Creating the boards in the heap and initialising to a random binary number.
	int **board = (int**)malloc(BOARD_DIM * sizeof(int *));
	int **board_new = (int**)malloc(BOARD_DIM * sizeof(int *));

	for(int i = 0; i < BOARD_DIM; i++) {
		board[i] = (int *)malloc(BOARD_DIM * sizeof(int));
		board_new[i] = (int *)malloc(BOARD_DIM * sizeof(int));

		for(int j = 0; j < BOARD_DIM; j++) {
			int random_number = rand() % 2;
			board[i][j] = random_number;
			board_new[i][j] = random_number;
		}
	}

	// Looping through each turn.
	for (int turn = 0; turn < TURNS; turn++) {

		// Print grid
		//print_grid(board);

		// Write grid to file for turns 1, 2 and 3.
		//if (turn >= 0 && turn <= 2) {
		//	writeGridToFile(board, turn);
		//}

		// Parralelise code using collapse and specify num_threads
		#pragma omp parallel for collapse(2) num_threads(NUM_THREADS)
		for (int i = 0; i < BOARD_DIM; i++) {
			for (int j = 0; j < BOARD_DIM; j++) {

				// Set up coordinates for counting neighbours.
				int iUp = mod (i-1, BOARD_DIM);
				int iDown = mod (i+1, BOARD_DIM);
			    int jLeft = mod (j-1, BOARD_DIM);
			    int jRight = mod (j+1, BOARD_DIM);

			    // Count neighbours for current cell.
				int neighbours = 
				board[iUp][j] + board[iDown][j] + 
				board[i][jLeft] + board[i][jRight];

				// Perform game of life operations on current cell.
				if (board[i][j] == 1 && neighbours < 2) {
					board_new[i][j] = 0;
				}
				if (board[i][j] == 1 && (neighbours == 2 || neighbours == 3)) {
					board_new[i][j] = 1;
				}
				if (board[i][j] == 1 && neighbours > 3) {
					board_new[i][j] = 0;
				}
				if (board[i][j] == 0 && neighbours == 3) {
					board_new[i][j] = 1;
				}
			}
		}

		//Copy the new board onto current board
		for (int x = 0; x < BOARD_DIM; x++) {
			for (int y = 0; y < BOARD_DIM; y++) {
				board[x][y] = board_new[x][y];
			}
		}
	}
	freeArray(board);
	freeArray(board_new);
}

void non_parallel() {

	// Seed the random number.
	srand(time(NULL));

	// Creating the boards in the heap and initialising to a random binary number.
	int **board = (int**)malloc(BOARD_DIM * sizeof(int *));
	int **board_new = (int**)malloc(BOARD_DIM * sizeof(int *));

	for(int i = 0; i < BOARD_DIM; i++) {
		board[i] = (int *)malloc(BOARD_DIM * sizeof(int));
		board_new[i] = (int *)malloc(BOARD_DIM * sizeof(int));

		for(int j = 0; j < BOARD_DIM; j++) {
			int random_number = rand() % 2;
			board[i][j] = random_number;
			board_new[i][j] = random_number;
		}
	}

	// Looping through each turn.
	for (int turn = 0; turn < TURNS; turn++) {

		// Print grid
		//print_grid(board);

		for (int i = 0; i < BOARD_DIM; i++) {
			for (int j = 0; j < BOARD_DIM; j++) {

				// Set up coordinates for counting neighbours.
				int iUp = mod (i-1, BOARD_DIM);
				int iDown = mod (i+1, BOARD_DIM);
			    int jLeft = mod (j-1, BOARD_DIM);
			    int jRight = mod (j+1, BOARD_DIM);

			    // Count neighbours for current cell.
				int neighbours = 
				board[iUp][j] + board[iDown][j] + 
				board[i][jLeft] + board[i][jRight];

				// Perform game of life operations on current cell.
				if (board[i][j] == 1 && neighbours < 2) {
					board_new[i][j] = 0;
				}
				if (board[i][j] == 1 && (neighbours == 2 || neighbours == 3)) {
					board_new[i][j] = 1;
				}
				if (board[i][j] == 1 && neighbours > 3) {
					board_new[i][j] = 0;
				}
				if (board[i][j] == 0 && neighbours == 3) {
					board_new[i][j] = 1;
				}
			}
		}

		//Copy the new board onto current board
		for (int x = 0; x < BOARD_DIM; x++) {
			for (int y = 0; y < BOARD_DIM; y++) {
				board[x][y] = board_new[x][y];
			}
		}
	}
	freeArray(board);
	freeArray(board_new);	
}

//AS THE BOARD DIMENSIONS INCREASE HOW DOES TIME CHANGE FOR EACH IMPLEMENTATION
void experiment_1() {

	FILE *f = fopen("experiment_1.txt", "w");
	if (f == NULL) {
	    printf("Error opening file!\n");
	    exit(EXIT_FAILURE);
	}

	for(int i = 0; i < 5; i++) {

		// Board dimensions = 128, 256, 512, 1024, 2048
		BOARD_DIM = pow(2,i) * 128;
		struct timeval start, end;

		printf("Board dimenions = %d x %d\n", BOARD_DIM, BOARD_DIM);

		//NON-PARALLEL
		gettimeofday(&start, NULL);
		non_parallel();
		gettimeofday(&end, NULL);
		float non_parallel_time = ((end.tv_sec  - start.tv_sec) * 1000000u +
        end.tv_usec - start.tv_usec) / 1.e6;

		printf("None parallel time = %.7f\n", non_parallel_time);
		fprintf(f,"%.5f ", non_parallel_time);

		//PARALLEL 1
		gettimeofday(&start, NULL);
		parallel1();
		gettimeofday(&end, NULL);
		float parallel_time_1 = ((end.tv_sec  - start.tv_sec) * 1000000u +
        end.tv_usec - start.tv_usec) / 1.e6;
		
		printf("Parallel time = %.7f\n", parallel_time_1);
        fprintf(f,"%.5f ", parallel_time_1);

        //PARALLEL 2
		gettimeofday(&start, NULL);
		parallel2();
		gettimeofday(&end, NULL);
		float parallel_time_2 = ((end.tv_sec  - start.tv_sec) * 1000000u +
        end.tv_usec - start.tv_usec) / 1.e6;

        printf("Parallel 2 time = %.7f \n\n", parallel_time_2);
        fprintf(f,"%.5f \n", parallel_time_2);
	}
	fclose(f);
}

//AS THE NUMBER OF TURNS INCREASE HOW DOES THE TIME CHANGE FOR EACH IMPLEMENTATION
void experiment_2(int *turns) {

	FILE *f = fopen("experiment_2.txt", "w");
	if (f == NULL) {
	    printf("Error opening file!\n");
	    exit(EXIT_FAILURE);
	}

	for(int i = 0; i < 5; i++) {

		// Number of turns = 100, 500, 1000, 2000, 4000
		TURNS = turns[i];
		struct timeval start, end;

		printf("Number of turns = %d\n", TURNS);

		//NON-PARALLEL
		gettimeofday(&start, NULL);
		non_parallel();
		gettimeofday(&end, NULL);
		float non_parallel_time = ((end.tv_sec  - start.tv_sec) * 1000000u +
        end.tv_usec - start.tv_usec) / 1.e6;

		printf("Non parallel time = %.7f\n", non_parallel_time);
		fprintf(f,"%.5f ", non_parallel_time);

		//PARALLEL 1
		gettimeofday(&start, NULL);
		parallel1();
		gettimeofday(&end, NULL);
		float parallel_time_1 = ((end.tv_sec  - start.tv_sec) * 1000000u +
        end.tv_usec - start.tv_usec) / 1.e6;

        printf("Parallel 1 time = %.7f\n", parallel_time_1);
        fprintf(f,"%.5f ", parallel_time_1);

        //PARALLEL 2
		gettimeofday(&start, NULL);
		parallel2();
		gettimeofday(&end, NULL);
		float parallel_time_2 = ((end.tv_sec  - start.tv_sec) * 1000000u +
        end.tv_usec - start.tv_usec) / 1.e6;

        printf("Parallel 2 time = %.7f\n\n", parallel_time_2);
        fprintf(f,"%.5f \n", parallel_time_2);
	}
	fclose(f);
}

//AS YOU CHANGE THE NUMBER OF THREADS HOW DOES THE TIME CHANGE FOR EACH IMPLEMENTATION
void experiment_3() {

	FILE *f = fopen("experiment_3.txt", "w");
	if (f == NULL) {
	    printf("Error opening file!\n");
	    exit(EXIT_FAILURE);
	}

	for(int i = 0; i < 10; i++) {

		// Number of threads = 1 though 10.
		NUM_THREADS = i+1;
		struct timeval start, end;

		printf("Number of threads = %d\n", NUM_THREADS);

		//PARALLEL 1
		gettimeofday(&start, NULL);
		parallel1();
		gettimeofday(&end, NULL);
		float parallel_time_1 = ((end.tv_sec  - start.tv_sec) * 1000000u +
        end.tv_usec - start.tv_usec) / 1.e6;

        printf("Parallel 1 time = %12.7f\n", parallel_time_1);
        fprintf(f,"%.5f ", parallel_time_1);

        //PARALLEL 2
        gettimeofday(&start, NULL);
		parallel2();
		gettimeofday(&end, NULL);
		float parallel_time_2 = ((end.tv_sec  - start.tv_sec) * 1000000u +
        end.tv_usec - start.tv_usec) / 1.e6;

        printf("Parallel 2 time = %12.7f\n\n", parallel_time_2);
        fprintf(f,"%.5f \n", parallel_time_2);
	}
	fclose(f);
}


int main() {

	//COMPUTER SPECS
		//NO OF CORES = 2
		//NO OF LOGICAL PROCESSES = 4
		//L1 CHACHE = 128KB
		//L2 CHACHE = 512KB
		//L3 CHACHE = 4MB

	//INITIAL ASSUMPTIONS
		//BOARD_DIM = 1024;
		//TURNS = 100;
		//NUM_THREADS = 4;

	//#1
	//AS THE BOARD DIMENSIONS INCREASE HOW DOES TIME CHANGE FOR EACH IMPLEMENTATION
	printf("Experiment 1: \n");
	TURNS = 100;
	NUM_THREADS = 4;
	experiment_1();

	//#2
	//AS THE NUMBER OF CYCLES INCREASE HOW DOES THE TIME CHANGE FOR EACH IMPLEMENTATION
	printf("Experiment 2: \n");
	BOARD_DIM = 1024; //This will take a long time to run
	int turns[5] = {100, 500, 1000, 2000, 4000};
	NUM_THREADS = 4;
	experiment_2(turns);
	
	//#3
	//AS YOU CHANGE THE NUMBER OF THREADS HOW DOES THE TIME CHANGE FOR EACH IMPLEMENTATION
	printf("Experiment 3: \n");
	BOARD_DIM = 1024;
	TURNS = 100;
	experiment_3();

	return EXIT_SUCCESS;
}