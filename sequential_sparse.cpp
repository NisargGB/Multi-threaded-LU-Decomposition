#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>
#include <chrono>
#include <vector>
#include "../include/functions.h"

using namespace std;
#define EPSILON 1e-9

// this function takes matrices (as array of n pointers), and dimension of the matrix as input and calculates the "l" and "u" matrices following the algorithm sequentially
vector<double**> sequentialDecomposition(vector<double**> input, int n)
{
    double** a = input[0];
    double** l = input[1];
    double** u = input[2];
    double** pi = input[3];

    // Sequential Algorithm

    for(int k=0 ; k<n ; k++)
    {
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
        
        if(max < EPSILON)               // since we are doing double comparisons so instead of checking max = 0.0, we did max < EPSILON defined above as 1e-9
        {
            printf("\n\nSingular matrix ERROR\nProgram terminated with code 1\n\n");
            cout << k << '\n';
            return {};
        }

        int temp0;
        temp0 = pi[0][k];
        pi[0][k] = pi[0][kd];
        pi[0][kd] = temp0;

        double temp1;
        for(int i=0 ; i<n ; i++)
        {
            temp1 = a[k][i];
            a[k][i] = a[kd][i];
            a[kd][i] = temp1;
        }

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