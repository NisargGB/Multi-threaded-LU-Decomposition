# Multi-threaded-LU-Decomposition
Three different implementations for computing LU decomposition of a matrix by Gaussian Elimination: Sequential, Parallelized using OpenMP, Parallelized using Pthreads

Usage -
  Compilation
    g++ sequential_implementation.cpp -o Sequential
    g++ checker.cpp -o Checker
  Running
    .\Sequential <size> <num_threads>
    .\Checker <size> <filename>
