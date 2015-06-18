# Multithreaded-Sudoku-Validator

This program reads from a file possible solutions to sudoku puzzles and evaluates each sudoku grid using 11 evaluation threads to determine whether the provided solutions are valid or invalid. The evaluation threads can be divided into the following categories:
```sh
  -> 9 threads to evaluate the 9 subgrids of the sudoku puzzle
  -> 2 threads to evaluate the row and columns of the sudoku puzzle
```
  
Group members: Eric Smith, Caitlin Facchina, Darko Atanackovic and Maher Husein

### Installation

```sh
$ git clone https://github.com/ambi0/Multithreaded-Sudoku-Validator Multithreaded-Sudoku-Validator
$ cd Multithreaded-Sudoku-Validator
```

### To Compile and Execute

```sh
$ gcc -o sudoku_validator sudoku_validator.c -lpthread
$ ./sudoku_validator input.txt
```

### Input Structure
```sh
3
295743861
431865927
876192543
387459216
612387495
549216738
763524189
928671354
154938672
295743861
431865922
876192543
387459216
612387495
549216738
763524189
928671354
254938672
295743861
431865927
876192543
387459216
612387495
549216738
763524189
928671354
154938672
```

### Tech

Multithreaded Sudoku Validator uses POSIX pthreads to determine the validity of possible solutions to sudoku puzzles.

### Version
1.0.0

### Todo's

Write Tests
