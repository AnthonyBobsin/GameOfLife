#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* readGrid, Function that reads the grid from the file and assigns it to the current_gen grid.
	FILE *fp, file to read from.
	int **current_gen, grid to write the file to.
	int N, number of rows or columns.
*/
void readGrid(FILE *fp, int **current_gen, int N) {
	int i, j;
	char ch;

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

/* initPrintGrid, Function to print out the starting grid in the correct format.
	int **grid, the grid to be printed.
	int N, number of rows or columns.
*/
void initPrintGrid(int **grid, int N) {
	int i, j;
	for (i = 0; i < N; i++) {
	    for (j = 0; j < N; j++) {
	        printf("%d ", grid[i][j]);
	    }
	    printf("\n");
	}
	sleep(1);
	system("clear");
}

/* getNext, Function to calculate the next generation and assign it to the current generation grid.
	int **current_gen, current grid to base next generation off of.
	int N, number of rows or columns.
*/
void getNext(int **current_gen, int N) {
	int **next_gen;
	int i, j, x, y;
	// Allocate memory for next_gen grid
	next_gen = malloc(N * sizeof(int *));
	for (i = 0; i < N; i++) {
    	next_gen[i] = malloc(N * sizeof(int));
    }
    //Loop through each position in grid
	for (i = 0; i < N; i++) {
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
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			current_gen[i][j] = next_gen[i][j];
		}
	}
	//Free space allocated for next_gen
	for (i = 0; i < N; ++i) {
	    free(next_gen[i]);
	}
	free(next_gen);
}

/* printGrid, Function to print the grid for each generation in the correct format.
	int **grid, the grid to be printed.
	int N, number of rows or columns.
*/
void printGrid(int **grid, int N) {
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
	system("clear");
}

int main( int argc, char *argv[] ) {

	int **current_gen;
	int N, i;
	char str[5];
	char ch;
	FILE *fp;

	//Invalid number of arguments
	if (argc != 2) {
		printf( "usage: %s filename", argv[0] );
	}
	else {
		char* filename = argv[1];
		fp = fopen(filename, "r");
		if( fp == NULL ) {
		    perror("Error while opening the file.\n");
		    exit(EXIT_FAILURE);
	    }

	    fgets(str, 5, fp);
	    N = atoi(str);

	    //Allocate memory for current_gen grid
	    current_gen = malloc(N * sizeof(int *));
	    for (i = 0; i < N; i++) {
	    	current_gen[i] = malloc(N * sizeof(int));
	    }

	    readGrid(fp, current_gen, N);
	    initPrintGrid(current_gen, N);
	 
	    fclose(fp);

	    while (1) {
	    	getNext(current_gen, N);
	    	printGrid(current_gen, N);
	    }
	}
}

