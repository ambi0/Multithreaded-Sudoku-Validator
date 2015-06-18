/* Multithreaded Sudoku Validator
 *
 * This program reads from a file possible solutions to sudoku puzzles and evaluates each sudoku grid
 * using 11 evaluation threads to determine whether the provided solutions are valid or invalid.
 * The evaluation threads can be divided into the following categories:
 * 		-> 9 threads to evaluate the 9 subgrids of the sudoku puzzle
 * 		-> 2 threads to evaluate the row and columns of the sudoku puzzle
 *
 * Group members: Eric Smith, Caitlin Facchina, Darko Atanackovic and Maher Husein
 *
 * Copyright (c) 2015
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

typedef struct{
	int row;
	int col;
	int (*sudoku_grid)[9];
}parameters;

void *validateSubgrid(void *);
void *validateRows(void *);
void *validateCols(void *);
int readSudokuGrid(int (*grid)[9], int, FILE *);

#define NTHREADS 11

int main(int argc, char *argv[]){
	if(argc < 2){
		fprintf(stderr, "Usage: executable-name [input-file]\n");
		exit(EXIT_FAILURE);
	}

	int sudoku_grid[9][9];
	int n_grids;
	FILE *fp = fopen(argv[1], "r");

	// Read number of sudoku grids to validate
	fscanf(fp,"%d",&n_grids);
	fseek(fp, 1, SEEK_CUR); // Seek past newline

	// Initalize parameters for subgrid evaluation threads
	parameters *data[9];
	int row, col, i = 0;
	for(row = 0; row < 9; row += 3)
	{
		for(col = 0; col < 9; col += 3, ++i)
		{
			data[i] = (parameters *)malloc(sizeof(parameters));
			if(data[i] == NULL){
				int err = errno;
				puts("malloc failed");
				puts(strerror(err));
				exit(EXIT_FAILURE);
			}
			data[i]->row = row;
			data[i]->col = col;
			data[i]->sudoku_grid = sudoku_grid;
		}
	}

	// Validate all sudoku grids in file
	pthread_t tid[NTHREADS];
	int g, p, j, h, retCode, check_flag = 0, t_status[NTHREADS];
	for(g = 1; g <= n_grids; ++g){
		if(readSudokuGrid(sudoku_grid, g, fp)){
			puts("something happened reading the grid from the file");
			exit(EXIT_FAILURE);
		}

		// Create threads for subgrid validation 
		for(p = 0; p < 9; ++p){
			if(retCode = pthread_create(&tid[p], NULL, validateSubgrid, (void *)data[p])){
				fprintf(stderr, "Error - pthread_create() return code: %d\n", retCode);
				exit(EXIT_FAILURE);
			}
		}


		// Create threads for row and column validation
		if(retCode = pthread_create(&tid[9], NULL, validateRows, (void *)data[0])){
			fprintf(stderr, "Error - pthread_create() return code: %d\n", retCode);
			exit(EXIT_FAILURE);
		}
		if(retCode = pthread_create(&tid[10], NULL, validateCols, (void *)data[0])){
			fprintf(stderr, "Error - pthread_create() return code: %d\n", retCode);
			exit(EXIT_FAILURE);
		}

		// Join all threads so main waits until the threads finish
		for(j = 0; j < NTHREADS; ++j){
			if(retCode = pthread_join(tid[j], (void *)&t_status[j])){
				fprintf(stderr, "Error - pthread_join() return code: %d\n", retCode);
				exit(EXIT_FAILURE);
			}
		}

		// Check the status returned by each thread
		for(h = 0; h < NTHREADS; ++h){
			if(t_status[h]){
				check_flag = 1;
				break;
			}
		}
		if(check_flag){
			printf("Sudoku Puzzle: %d - Invalid\n", g);
		}else{
			printf("Sudoku Puzzle: %d - Valid\n", g);
		}
		check_flag = 0;
	} // End evaluation for loop

	// Free memory and close the file
	int k;
	for(k = 0; k < 9; ++k){
		free(data[k]);
	}
	fclose(fp);

	return 0;
}

/**
     * Checks if the subgrid of the sudoku grid is valid.
     * 
     * @param data
     *      Contains the sudoku grid and the starting row and column
     *		of the subgrid to test.
     */
void *validateSubgrid(void *data){
	int digit_check[10] = {0};
	parameters *params = (parameters *)data;
	int i, j;
	for(i = params->row; i < params->row + 3; ++i){
		for(j = params->col; j < params->col + 3; ++j){
			if(digit_check[params->sudoku_grid[i][j]] == 1){
				return (void *)-1; // Invalid sudoku subgrid
			}
			digit_check[params->sudoku_grid[i][j]] = 1;
		}
	}
	return (void *)0; // Valid sudoku subgrid
}

/**
     * Checks whether the rows of the sudoku grid are valid.
     * 
     * @param data
     *      Contains the sudoku grid to test.
     */
void *validateRows(void *data){
	int digit_check[10] = {0};
	parameters *params = (parameters *)data;
	int i, j;
	for(i = 0; i < 9; ++i){
		for(j = 0; j < 9; ++j){
			if(digit_check[params->sudoku_grid[i][j]] == 1){
				return (void *)-1; // Invalid sudoku rows
			}
			digit_check[params->sudoku_grid[i][j]] = 1;
		}
		// Reinitalize check array for next row
		memset(digit_check, 0, sizeof(int)*10);
	}
	return (void *)0; // Valid sudoku rows
}

/**
     * Checks whether the columns of the sudoku grid are valid.
     * 
     * @param data
     *      Contains the sudoku grid to test.
     */
void *validateCols(void *data){
	int digit_check[10] = {0};
	parameters *params = (parameters *)data;
	int i, j;
	for(i = 0; i < 9; ++i){
		for(j = 0; j < 9; ++j){
			if(digit_check[params->sudoku_grid[j][i]] == 1){ // Ensure the digit doesn't appear more than once in the subgrid
				return (void *)-1; // Invalid sudoku columns
			}
			digit_check[params->sudoku_grid[j][i]] = 1;
		}
		// Reinitalize check array for next column
		memset(digit_check, 0, sizeof(int)*10);
	}
	return (void *)0; // Valid sudoku columns
}

/**
     * Reads in a sudoku grid from a file.
     * 
     * @param grid
     *      Where to store the grid.
     * @param grid_no 
     *      Grid number to read from file.
     * @param fp 
     *      File to read from.
     */
int readSudokuGrid(int (*grid)[9], int grid_no, FILE *fp){ //make sure valid entires within sudoku grid
	int garbage;
	fseek(fp, 0, SEEK_SET);
	fscanf(fp, "%d", &garbage);
	fseek(fp, 1, SEEK_CUR); // Seek to start of first sudoku grid

	if(grid_no < 1){
		puts("Please specify a lol");
		return 0;
	}
	else if(grid_no > 1){ // Seek past newlines from previous grids
		fseek(fp, 9*(grid_no - 1), SEEK_CUR); // 10 newlines per grid when more than one grid
	}

	fseek(fp, (grid_no - 1)*sizeof(char)*81, SEEK_CUR); // Seek to the start of the corresponding grid number

	char entry;
	int i = 0, j = 0, totalValues = 0;
	while((fread(&entry, 1, 1, fp)) > 0 && totalValues < 81){ // Read 81 digits from file: sudoku grid 9x9 = 81
		if(entry != '\n'){ // Ignore newline
			if(isdigit(entry)){
				++totalValues;
				grid[i][j] = entry - '0'; // Store integer representation
				++j;
				if(j == 9){
					j = 0;
					++i;
				}
			}
			else{ // A non-digit character was read
				return -1;
			}
		}
	}

	return 0; // Successfully read sudoku grid from file
}