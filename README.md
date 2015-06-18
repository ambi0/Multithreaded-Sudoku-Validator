# Multithreaded-Sudoku-Validator

This program reads from a file possible solutions to sudoku puzzles and evaluates each sudoku grid using 11 evaluation threads to determine whether the provided solutions are valid or invalid. The evaluation threads can be divided into the following categories:
  -> 9 threads to evaluate the 9 subgrids of the sudoku puzzle
  -> 2 threads to evaluate the row and columns of the sudoku puzzle
  
Group members: Eric Smith, Caitlin Facchina, Darko Atanackovic and Maher Husein

### To Compile...

### Input Structure

### Version
1.0.0

### Tech

Multithreaded Sudoku Validator uses POSIX pthreads to determine the validity of possible solutions to sudoku puzzles.

### Installation

```sh
$ git clone https://github.com/ambi0/Multithreaded-Sudoku-Validator Multithreaded-Sudoku-Validator
$ cd Multithreaded-Sudoku-Validator
```

### Todo's

Write Tests
