#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>
#include <chrono>
#include <fstream>
#include <math.h>
#include <random>
#include <omp.h>
#include <vector>
#include "../include/functions.h"

using namespace std;

int n;// dimension of the matrix
int t;  // number of threads


int main(int argc, char *argv[])
{
    // Command line args: <size of matrix> <sequential/openmp/pthreads> <num_threads> <bool: check_norm> <bool: check_mult>
    
    auto start = chrono::high_resolution_clock::now();      //used chrono to track the time

    // Command line args parsing
    if(argc != 6)
    {
        printInputError();
        return -1;
    }

    try
    {
        string n_str(argv[1]);
        n = stoi(n_str);                        // Dimension of the square matrix
    }
    catch(const std::exception& e)
    {
        cout << "There was an error parsing the input: argument1 needs to be integer" << '\n';
        return -1;
    }
    
    string mode(argv[2]);

    try
    {
        string t_str(argv[3]);                   // Number of threads
        t = stoi(t_str);
    }
    catch(const std::exception& e)
    {
        cout << "There was an error parsing the input: argument3 needs to be integer" << '\n';
        return -1;
    }
                           
    string checkNorm_str(argv[4]);
    bool checkNorm;                         // Whether to check norm of A residual
    if(checkNorm_str == "true")
        checkNorm = true;
    else if(checkNorm_str == "false")
        checkNorm = false;
    else
    {
        printInputError();
        return -1;
    }
    string checkMult_str(argv[5]);
    bool checkMult;                         // Whether to check norm of PA - LU
    if(checkMult_str == "true")
        checkMult = true;
    else if(checkMult_str == "false")
        checkMult = false;
    else
    {
        printInputError();
        return -1;
    }
    
    double* a;      //the DENSE implementation: a contiguous array
    double* l;
    double* u;
    double* pi;
    vector<double*> matrices = initialiseMatricesDense(n);      //initialises the matries a, l and u using Uniform Real Distribution
    a = matrices[0];
    l = matrices[1];
    u = matrices[2];
    pi = matrices[3];
    // printMatrixDense(a, n, "Target");     //function to print the matrix on the console
    // printMatrixDense(l, n, "Lower");
    // printMatrixDense(u, n, "Upper");
    vector<int> thread_vec = {1, 2, 4, 6, 8, 12, 16, 32};
    
    vector<double*> output;
    vector<double*> input = {a, l, u, pi};
    
    if(checkMult)
    {
        saveMatrixDense(a, n, "ALU.txt", "a");      //saves the matrix in row major format in file named "ALU.txt"
    }

    if(mode == "sequential")
    {
        output = sequentialDecompositionDense(input, n);    //for carrying out the program sequentially.
    }
    else if(mode == "openmp")
    {
        // for(int x : thread_vec)         //for checking performance of multiple threads
        // {
        //     output = openmpDecompositionDense(input, n, x);
        //     auto end = chrono::high_resolution_clock::now();
        //     auto time_taken = chrono::duration_cast<chrono::milliseconds>(end - start);
        //     printf("\nTime taken: %.02fs, thread num: %d\n", (float)time_taken.count()/1000, x);
        //     free(a);
        //     free(l);
        //     free(u);
        //     free(pi);
        //     start = chrono::high_resolution_clock::now();
        //     matrices = initialiseMatricesDense(n);
        //     a = matrices[0];
        //     l = matrices[1];
        //     u = matrices[2];
        //     pi = matrices[3];
        //     input = {a, l, u, pi};
        // }
        output = openmpDecompositionDense(input, n, t);      //for carrying out the program using openmp
    }
    else if(mode == "pthreads")
    {
        // for(int x : thread_vec)             //for checking performance of multiple threads
        // {
        //     output = pthreadsDecomposition(input, n, x);
        //     auto end = chrono::high_resolution_clock::now();
        //     auto time_taken = chrono::duration_cast<chrono::milliseconds>(end - start);
        //     printf("\nTime taken: %.02fs, thread num: %d\n", (float)time_taken.count()/1000, x);
        //     free(a);
        //     free(l);
        //     free(u);
        //     free(pi);
        //     start = chrono::high_resolution_clock::now();
        //     matrices = initialiseMatrices(n);
        //     a = matrices[0];
        //     l = matrices[1];
        //     u = matrices[2];
        //     pi = matrices[3];
        //     input = {a, l, u, pi};
        // }
        output = pthreadsDecompositionDense(input, n, t);       //for carrying out the program using pthreads

    }
    else
    {
        printInputError();      // prints an error if input is not in desired format: <size of matrix> <sequential/openmp/pthreads> <num_threads> <bool: check_norm> <bool: check_mult>
        return -1;
    }
    
    if(output.empty())
    {
        return -1;
    }

    if(checkMult)
    {
        saveMatrixDense(a,n, "ALU.txt", "ares");        //in the same file "ALU.txt" saves A residual, then L, then U and then P.
        saveMatrixDense(l,n, "ALU.txt", "l");
        saveMatrixDense(u,n, "ALU.txt", "u");
        saveMatrixDense(pi,n, "ALU.txt", "pi");
    }
    
    if(checkNorm)
    {
        double l21 = -1;
        l21 = l21normDense(a, n);       //function to check the l21 norm of a matrix
        cout << "\nL2,1 Norm of the matrix = " << l21 << "\n";
    }

    // printMatrixDense(a, n, "Residual_matrix");
    // printMatrixDense(l, n, "Lower_out");
    // printMatrixDense(u, n, "Upper_out");
    // printMatrixDense(pi, n, "pi");

    auto end = chrono::high_resolution_clock::now();
    auto time_taken = chrono::duration_cast<chrono::milliseconds>(end - start);     //the total time required for the complete execution of the program
    printf("\nTime taken: %.02fs\n", (float)time_taken.count()/1000);
    
    return 0;
}