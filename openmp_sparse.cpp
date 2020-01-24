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

// #define RAND_MAX 1000

//this function takes the matrix a, dimension of matrix n and number of threads as input and uses openmp to calculate the l and u matrices.
vector<double**> openmpDecomposition(vector<double**> input, int n, int t)
{
    double** a = input[0];
    double** l = input[1];
    double** u = input[2];
    double** pi = input[3];

    for(int k=0 ; k<n ; k++)    //the outer for loop which updates the kth row and kth column of the matrix a.
    {
        // finding max element
        double max = 0.0;   
        int kd = -1;
        for(int i=k; i<n ; i++) //for calculating the max value's index kd
        {
            if(max < abs(a[i][k]))
            {
                max = abs(a[i][k]);
                kd = i;
            }
        }
        
        if(kd == -1)
        {
            printf("\n\nSingular matrix ERROR\nProgram terminated with code 1\n\n");
            // cout << k << '\n';
            return {};
        }

        //updating the pi vector to store the permutation matrix P
        int temp0;  
        temp0 = pi[0][k];
        pi[0][k] = pi[0][kd];
        pi[0][kd] = temp0;

        // swap a(k,:) and a(kd,:)
        double temp1;
        for(int i=0 ; i<n ; i++)
        {
            temp1 = a[k][i];
            a[k][i] = a[kd][i];
            a[kd][i] = temp1;
        }
        
        // swap l(k,1:k-1) and l(kd,1:k-1)
        for(int i=0 ; i<k ; i++)
        {
            temp1 = l[k][i];
            l[k][i] = l[kd][i];
            l[kd][i] = temp1;
        }
    
        u[k][k] = a[k][k];

        for(int i=k+1 ; i<n ; i++)
        {
            l[i][k] = a[i][k] / u[k][k];
            u[k][i] = a[k][i];
        }

        // int thread_dash = ceil((float)t*(1.0 - ((float)(k*k)/(float)(n*n))));
        // thread_dash = std::max(thread_dash, 1);
        int thread_dash = t;
        
        //parallelized the double for loop as it has no data dependency between various i and j indices. used collapse(2) to tell 
        //openmp to parallelize the nested loops and used schedule(guided, 2) to distribute the work in guided fashion where size of
        //the chunk is given proportional to the unassigned iterations.
        #pragma omp parallel for num_threads(thread_dash) default(none) shared(k,l,u,a,n) collapse(2)
        for(int i=k ; i<n ; i++)
        {
            for(int j=k ; j<n ; j++)
            {
                a[i][j] = a[i][j] - l[i][k] * u[k][j];
            }
        }
    }

    return {a, l, u, pi};
}
