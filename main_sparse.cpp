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

// #define RAND_MAX 1000 TODO

int n;
int t;


int main(int argc, char *argv[])
{
    // Command line args: <size of matrix> <sequential/openmp/pthreads> <num_threads> <bool: check_norm> <bool: check_mult>
    
    auto start = chrono::high_resolution_clock::now();  //used chrono to track the time

    // Command line args parsing
    if(argc != 6)
    {
        printInputError();
        return -1;
    }

    string n_str(argv[1]);
    n = stoi(n_str);                        // Dimension of the square matrix
    string mode(argv[2]);
    string t_str(argv[3]);
    t = stoi(t_str);                        // Number of threads
    string checkNorm_str(argv[4]);
    bool checkNorm;                         // Whether to check l21 norm of A residual
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
    
    double** a; //the SPARSE implementation: an array of n pointers.
    double** l;
    double** u;
    double** pi;
    vector<double**> matrices = initialiseMatrices(n);  //initialises the matries a, l and u using Uniform Real Distribution
    a = matrices[0];
    l = matrices[1];
    u = matrices[2];
    pi = matrices[3];
    printMatrix(a, n, "Target");    //function to print the matrix on the console
    printMatrix(l, n, "Lower");
    printMatrix(u, n, "Upper");

    
    vector<double**> output;
    vector<double**> input = {a, l, u, pi};
    
    if(checkMult)
    {
        saveMatrix(a, n, "ALU.txt", "a");   //saves the matrix in row major format in file named ALU.txt
    }

    if(mode == "sequential")
    {       
        output = sequentialDecomposition(input, n); //for carrying out the program sequentially.
    }
    else if(mode == "openmp")
    {
        output = openmpDecomposition(input, n, t);  //for carrying out the program using openmp
    }
    else if(mode == "pthreads")
    {
        output = pthreadsDecomposition(input, n, t);    //for carrying out the program using pthreads
    }
    else
    {
        printInputError();
        return -1;
    }
    
    if(checkMult)
    {
        saveMatrix(a,n, "ALU.txt", "ares"); //in the same file "ALU.txt" saves A residual, then L, then U and then P.
        saveMatrix(l,n, "ALU.txt", "l");
        saveMatrix(u,n, "ALU.txt", "u");
        saveMatrix(pi,n, "ALU.txt", "pi");
    }
    
    if(checkNorm)
    {
        double l21 = -1;
        l21 = l21norm(a, n);    //function to check the l21 norm of a matrix
        cout << "\nL2,1 Norm of the matrix = " << l21 << "\n";
    }

    auto start2 = chrono::high_resolution_clock::now();

    printMatrix(a, n, "Residual_matrix");
    printMatrix(l, n, "Lower_out");
    printMatrix(u, n, "Upper_out");
    printMatrix(pi, n, "pi");

    auto end = chrono::high_resolution_clock::now();
    auto time_taken = chrono::duration_cast<chrono::milliseconds>(end - start); //the total time required for the complete execution of the program
    auto time_taken2 = chrono::duration_cast<chrono::milliseconds>(start2 - start);
    printf("\nTime taken: %.02fs, %.02fs\n", (float)time_taken.count()/1000, (float)time_taken2.count()/1000);
    
    // saveResidual(a, n, 1);
    return 0;
}
