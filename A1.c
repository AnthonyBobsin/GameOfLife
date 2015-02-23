/* Compile with 'gcc A1.c', then run with two arguments. 
	The first argument being the file where the grid is located.
	The second argument being how many threads you want to use.
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

/* Global Variables */

int **current_gen;
int N;
int rows;
int start;

/* readGrid, Function that reads the grid from the file and assigns it to the current_gen grid.
	FILE *fp, file to read from.
*/
void readGrid(FILE *fp) {
	int i, j;
	char ch;

	system("clear");
    printf("%d\n", N);
    //Loop through file to assign correct positions to grid
    for (i = 0; i < N; i++) {
    	for (j = 0; j < N; j++) {
	   		ch = fgetc(fp);
	   		//if ch is not new line or blank space, assign
	   		if (ch != '\n' && ch != ' ') {
	   			current_gen[i][j] = ch - '0';
	   		}
	   		//ch is a new line or black space
	   		else {
	   			//get next character
	   			ch = fgetc(fp);
	   			//if ch is not new line or blank space, assign
    			if (ch != '\n' && ch != ' ') {
       				current_gen[i][j] = ch - '0';
	   			}
	   			//ch is a new line or blank space
	   			else {
	   				//Get next char and assign
	   				ch = fgetc(fp);
	    			current_gen[i][j] = ch - '0';
	    		}
	    	}
	    }
	}
}

/* checkGrid, Function to handle error handling of grid format.
*/

void checkGrid() {
	int i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if (current_gen[i][j] != 0 && current_gen[i][j] != 1) {
				fprintf(stderr, "Error: Grid from file incorrect format\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

/* initPrintGrid, Function to print out the starting grid in the correct format.
*/
void initPrintGrid() {
	int i, j;
	for (i = 0; i < N; i++) {
	    for (j = 0; j < N; j++) {
	        printf("%d ", current_gen[i][j]);
	    }
	    printf("\n");
	}
	sleep(1);
}

/* getNext, Function to calculate the next generation and assign it to the current generation grid.
	int **current_gen, current grid to base next generation off of.
	int N, number of rows or columns.
*/
int getNext() {
	int **next_gen;
	int i, j, x, y, current_rows;
	// Allocate memory for next_gen grid
	next_gen = malloc(N * sizeof(int *));
	

    if ((N - start) < rows ){
    	current_rows = N;
    }
    else {
    	current_rows = start + rows;
    }
    for (i = start; i < current_rows; i++) {
    	next_gen[i] = malloc(N * sizeof(int));
    }

    //Loop through each position in grid
	for (i = start; i < current_rows; i++) {
		for (j = 0; j < N; j++) {
			int neighbours = 0;
			//Loop through all neighbours for current position
			for (x = i - 1; x <= i + 1; x++) {
				for (y = j - 1; y <= j + 1; y++) {
					//Determines if current position has a neighbour at each expected position
					if (current_gen[(x + N) % N][(y + N) % N]) {
						neighbours++;
					}
				}
			}
			//Current position was incremented if live, therefore have to clean up
			if (current_gen[i][j]) {
				neighbours--;
			}
			//Assigns position as live if live neighbours = 3 or if live neighbours = 2 and was previously live
			next_gen[i][j] = (neighbours == 3 || (neighbours == 2 && current_gen[i][j]));
		}
	}
	//Assign the calculated next generation grid to the current grid
	for (i = start; i < current_rows; i++) {
		for (j = 0; j < N; j++) {
			current_gen[i][j] = next_gen[i][j];
		}
	}
	//Free space allocated for next_gen
	for (i = start; i < current_rows; ++i) {
	    free(next_gen[i]);
	}
	free(next_gen);
	return 0;
}

/* printGrid, Function to print the grid for each generation in the correct format.
	int **grid, the grid to be printed.
	int N, number of rows or columns.
*/
void printGrid(int **grid, int N) {
	system("clear");
	int i, j;
	for (i = -1; i <= N; i++) {
		printf("-");
	}
	putchar('\n');
	for (i = 0; i < N; i++) {
		printf("|");
	    for (j = 0; j < N; j++) {
	    	//Print x if live, blank space if dead
	        printf(grid[i][j] ? "x" : " ");
	    }
	    printf("|");
	    printf("\n");
	}
	for (i = -1; i <= N; i++) {
		printf("-");
	}
	putchar('\n');
	sleep(1);
	
}

/* mainLoop, Function that runs the while loop for the game.
	int *stack_top, memory space to use for clone processes
	int threads, number of threads to divide the work between.
*/
void mainLoop(int threads) {

	const int STACK_SIZE = 65536;
	int *stack;
	int *stack_top;
	int pid, i;

	while (1) {
		stack = malloc(STACK_SIZE * sizeof(*stack));
	    if (stack == NULL) {
	        fprintf(stderr, "malloc");
	        exit(EXIT_FAILURE);
	    }
    	stack_top = stack + STACK_SIZE;  /* Assume stack grows downward */
	    start = 0;

	 	for (i = 1; i <= threads; i++) {
	    	//Clone with flag CLONE_VM to keep same memory space, and CLONE_VFORK to wait until child process is done
	    	pid = clone(getNext, stack_top, CLONE_VM|CLONE_VFORK, NULL);
	    	if (pid == -1) {
	    		exit(EXIT_FAILURE);
	    	}
	    	start = (N / threads) * i;
	    }
	    if ((N - start) < rows) {
	    	pid = clone(getNext, stack_top, CLONE_VM|CLONE_VFORK, NULL);
	    }
	    printGrid(current_gen, N);
	}
}

int main( int argc, char *argv[] ) {

	int i, threads;
	char str[5];
	char ch;
	FILE *fp;

	//Invalid number of arguments
	if (argc != 3) {
		fprintf(stderr, "Error: Invalid number of arguments.");
		exit(EXIT_FAILURE);
	}
	else {
		char* filename = argv[1];
		fp = fopen(filename, "r");
		if( fp == NULL ) {
		    fprintf(stderr, "Error while opening the file.\n");
		    exit(EXIT_FAILURE);
	    }

	    threads = atoi(argv[2]);
	    fgets(str, 5, fp);
	    N = atoi(str);
	    if (N == 0) {
	    	fprintf(stderr, "Error: Incorrect file format");
	    	exit(EXIT_FAILURE);
	    }

	    rows = (N / threads);

	    //Allocate memory for current_gen grid
	    current_gen = malloc(N * sizeof(int *));
	    for (i = 0; i < N; i++) {
	    	current_gen[i] = malloc(N * sizeof(int));
	    }

	    readGrid(fp);
	    checkGrid();
	    initPrintGrid();
	 
	    fclose(fp);

	    mainLoop(threads);
	}
}

