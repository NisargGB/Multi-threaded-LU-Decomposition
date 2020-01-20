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
#include "../include/functions.h"

using namespace std;

// #define RAND_MAX 1000


void printMatrix(double** matrix, int dim, string msg)
{
    cout << '\n' << msg << '\n';
    int rows = dim;
    if(msg=="pi")
        rows = 1;
    for(int i=0 ; i<rows ; i++)
    {
        for(int j=0 ; j<dim ; j++)
        {
            printf("%.02lf ", matrix[i][j]);
        }
        printf("\n");
    }
}



void saveMatrix(double** matrix, int dim, string filename, string mode)
{
    ofstream outfile;
    if(mode != "a")
        outfile.open(filename, std::ios_base::app);
    else
        outfile.open(filename);

    int rows = dim;
    if(mode == "pi")
        rows = 1;

    for(int i=0 ; i<rows ; i++)
    {
        for(int j=0 ; j<dim ; j++)
        {
            outfile << fixed << setprecision(15) << matrix[i][j] << " ";
        }
        outfile << '\n';
    }
    outfile.close();
}



void printInputError()
{
    cout << "\nERROR: Terminated with code -1\n\nThe program expects the following arguments (ordered)\n";
    cout << "<size of matrix> <sequential/openmp/pthreads> <num_threads> <bool: check_norm> <bool: check_mult>\n";
}



vector<double**> initialiseMatrices(int n)
{
    // Range of random numbers: TODO
    int range = 10;
    long factor = RAND_MAX / range;
                                               // Matrices A, L, U
    double** a = (double**)malloc(sizeof(double*)*(n));
    double** l = (double**)malloc(sizeof(double*)*(n));
    double** u = (double**)malloc(sizeof(double*)*(n));
                                                // Compact permutation matrix
    double** pi = (double**)malloc(sizeof(double*)*(1));
    pi[0] = (double*)malloc(sizeof(double)*(n));

    srand(time(0));

    // Initializations
    double* aarr;
    double* larr;
    double* uarr;
    for (int i=0 ; i<n ; i++)
    {
        pi[0][i] = i;
        aarr = (double*)malloc(sizeof(double)*(n));
        larr = (double*)malloc(sizeof(double)*(n));
        uarr = (double*)malloc(sizeof(double)*(n));
        for (int j=0 ; j<n ; j++)
        {
            aarr[j] = ((double)(rand()%1000)) / 100.0;
            // aarr[j] = 3*i + j + 1;
            //a[i*n + j] = 3*(i) + (j+1);
            // a[i*n + j] = rand()%10;
            if(j>i)
            {
               uarr[j] = aarr[j];
               larr[j] = 0.0;
            }
            else if(j==i)
            {
               uarr[j] = aarr[j];
               larr[j] = 1.0;
            }
            else
            {
               uarr[j] = 0.0;
               larr[j] = aarr[j];
            }
        }
        a[i] = aarr;
        l[i] = larr;
        u[i] = uarr;
    }

    return {a, l, u, pi};
}

double l21norm(double** matrix, int n)
{
    double l21 = 0.0;

    for(int j=0 ; j<n ; j++)
    {
        double temp = 0.0;
        for(int i=0 ; i<n ; i++)
        {
            temp += matrix[i][j] * matrix[i][j];
        }
        l21 += sqrt(temp);
    }

    return l21;
}
