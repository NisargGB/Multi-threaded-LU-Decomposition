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

vector<double**> openmpDecomposition(vector<double**> input, int n, int t)
{
    double** a = input[0];
    double** l = input[1];
    double** u = input[2];
    double** pi = input[3];

    // Parallelized loops in sequential Algorithm

    for(int k=0 ; k<n ; k++)
    {
        // finding max element
        double max = 0.0;
        int kd = -1;
        for(int i=k; i<n ; i++)
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

        int temp0;
        temp0 = pi[0][k];
        pi[0][k] = pi[0][kd];
        pi[0][kd] = temp0;

        // int linear_numThreads = ceil(sqrt(t));
        // #pragma omp parallel for num_threads(linear_numThreads) default(none) shared(k,kd,l,u,a,n) private(temp1)
        
        // swap a(k,:) and a(k',:)
        double temp1;
        for(int i=0 ; i<n ; i++)
        {
            temp1 = a[k][i];
            a[k][i] = a[kd][i];
            a[kd][i] = temp1;
        }
        
        // swap l(k,1:k-1) and l(k',1:k-1)
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
        // int cond = 1;
        // if((float)(n-k)/(float)n <= 0.2) cond = 0;
        // if(k%100==0) cout << thread_dash << '\n';
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
