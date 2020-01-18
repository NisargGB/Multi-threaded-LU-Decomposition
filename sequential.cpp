#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>
#include <chrono>
#include <fstream>
#include <omp.h>

using namespace std;

// #define RAND_MAX 1000

void printMatrix(double* matrix, int dim, string msg);
void saveResidual(double *matrix, int dim, int id);

int main(int argc, char *argv[])
{
    // Command line args: <size of matrix A>
    
    auto start = chrono::high_resolution_clock::now();

    string n_str(argv[1]);
    int n = stoi(n_str);                        // Dimension of the square matrix
    
    // Range of random numbers: TODO
    int range = 10;
    long factor = RAND_MAX / range;
    //double a[n][n], l[n][n], u[n][n];           // Matrices A, L, U
    double* a;
    a = (double*)malloc(sizeof(double)*(n*n));
    double* l;
    l = (double*)malloc(sizeof(double)*(n*n));
    double* u;
    u = (double*)malloc(sizeof(double)*(n*n));
    
    //int pi[n];                                  // Compact permutation matrix
    int* pi;
    pi = (int*)malloc(sizeof(int)*(n));

    srand(time(0));

    // Initializations
    for (int i=0 ; i<n ; i++)
    {
       pi[i] = i;
       for (int j=0 ; j<n ; j++)
       {
            a[i*n + j] = ((double)(rand()%1000)) / 100.0;
            //a[i*n + j] = 3*(i) + (j+1);
            // a[i*n + j] = rand()%10;
            if(j>i)
            {
               u[i*n + j] = a[i*n + j];
               l[i*n + j] = 0.0;
            }
            else if(j==i)
            {
               u[i*n + j] = a[i*n + j];
               l[i*n + j] = 1.0;
            }
            else
            {
               u[i*n + j] = 0.0;
               l[i*n + j] = a[i*n + j];
            }
        }
    }


    // printMatrix((double *)a, n, "Target");
    // printMatrix((double *)u, n, "Upper");
    // printMatrix((double *)l, n, "Lower");
    
    // Sequential Algorithm

    for(int k=0 ; k<n ; k++)
    {
        double max = 0.0;
        int kd = -1;
        for(int i=k; i<n ; i++)
        {
            if(max < abs(a[i*n + k]))
            {
                max = abs(a[i*n + k]);
                kd = i;
            }
        }
        
        if(kd == -1)
        {
            printf("\n\nSingular matrix ERROR\nProgram terminated with code 1\n\n");
            cout << k << '\n';
            return 1;
        }

        int temp0;
        temp0 = pi[k];
        pi[k] = pi[kd];
        pi[kd] = temp0;

        double temp1;
        for(int i=0 ; i<n ; i++)
        {
            temp1 = a[k*n + i];
            a[k*n + i] = a[kd*n + i];
            a[kd*n + i] = temp1;
        }

        for(int i=0 ; i<k ; i++)
        {
            temp1 = l[k*n + i];
            l[k*n + i] = l[kd*n + i];
            l[kd*n + i] = temp1;
        }

        u[k*n + k] = a[k*n + k];

        for(int i=k+1 ; i<n ; i++)
        {
            l[i*n + k] = a[i*n + k] / u[k*n + k];
            u[k*n + i] = a[k*n + i];
        }

        for(int i=k ; i<n ; i++)
        {
            for(int j=k ; j<n ; j++)
            {
                a[i*n + j] = a[i*n + j] - l[i*n + k] * u[k*n + j];
            }
        }
    }

    // printMatrix((double *)a, n, "Residual_matrix");
    // printMatrix((double *)u, n, "Upper_out");
    // printMatrix((double *)l, n, "Lower_out");
    // cout << "PI vector" << endl;
    // for(int m = 0; m < n; m++)
    // {
    //     cout << pi[m] << " ";
    // }

    auto end = chrono::high_resolution_clock::now();
    auto time_taken = chrono::duration_cast<chrono::milliseconds>(end - start);

    printf("\nTime taken: %.02fs\n", (float)time_taken.count()/1000);

    // saveResidual((double *)a, n, 0);
    return 0;
}


void printMatrix(double* matrix, int dim, string msg)
{
    cout << '\n' << msg << '\n';
    for(int i=0 ; i<dim ; i++)
    {
        for(int j=0 ; j<dim ; j++)
        {
            printf("%.02lf ", matrix[i*dim + j]);
        }
        printf("\n");
    }
}


void saveResidual(double* matrix, int dim, string filename)
{
    ofstream outfile;
    outfile.open(filename, std::ios_base::app);

    for(int i=0 ; i<dim ; i++)
    {
        for(int j=0 ; j<dim ; j++)
        {
            outfile << fixed << setprecision(15) << matrix[i*dim + j] << " ";
        }
        outfile << '\n';
    }
    outfile.close();
}