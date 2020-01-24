# Multi-threaded-LU-Decomposition
Three different implementations for computing LU decomposition of a matrix by Gaussian Elimination: Sequential, Parallelized using OpenMP, Parallelized using Pthreads

Usage -
  Compilation
    ./compile.sh
  Running
    ./run.sh <size of matrix> <sequential/openmp/pthreads> <num_threads> <bool: check_norm> <bool: check_mult>

To run the SPARSE implementation, run the LUDecomposer_sparse in run.sh and to run the DENSE implementation, run the LUDecomposer_dense in run.sh with same set of arguments.

For checking whether multiplication of LU is correct and norm of PA-LU is very small, use multiplier.py file in the src folder.
Run by: $python multiplier.py --n=<size of matrix> --alu==<filename>  
By default, the filename is "ALU.txt" and is formed in the bin folder.