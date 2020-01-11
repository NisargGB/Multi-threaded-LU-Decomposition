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
    // Command line args: <size of matrix A> <number of threads>
    
    auto start = chrono::high_resolution_clock::now();

    string n_str(argv[1]);
    int n = stoi(n_str);                        // Dimension of the square matrix
    string t_str(argv[2]);
    int t = stoi(t_str);                        // Number of threads

    // Range of random numbers: TODO
    int range = 10;
    long factor = RAND_MAX / range;
    double a[n][n], l[n][n], u[n][n];           // Matrices A, L, U
    int pi[n];                                  // Compact permutation matrix
    srand(time(0));

    // Initializations
    for (int i=0 ; i<n ; i++)
    {
       pi[i] = i;
       for (int j=0 ; j<n ; j++)
       {
            a[i][j] = ((double)(rand()%1000)) / 100.0;
            // a[i][j] = rand()%10;
            if(j>i)
            {
               u[i][j] = a[i][j];
               l[i][j] = 0.0;
            }
            else if(j==i)
            {
               u[i][j] = a[i][j];
               l[i][j] = 1.0;
            }
            else
            {
               u[i][j] = 0.0;
               l[i][j] = a[i][j];
            }
        }
    }

    printMatrix((double *)a, n, "Target");
    printMatrix((double *)u, n, "Upper");
    printMatrix((double *)l, n, "Lower");
    
    // Sequential Algorithm

    for(int k=0 ; k<n ; k++)
    {
        double max = 0.0;
        int kd = -1;
        for(int i=k-1 ; i<n ; i++)
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
            return 1;
        }

        int temp0;
        temp0 = pi[k];
        pi[k] = pi[kd];
        pi[kd] = temp0;

        double temp1;
        for(int i=0 ; i<n ; i++)
        {
            temp1 = a[k][i];
            a[k][i] = a[kd][i];
            a[kd][i] = temp1;
        }

        for(int i=0 ; i<k-1 ; i++)
        {
            temp1 = l[k][i];
            l[k][i] = l[kd][i];
            l[kd][i] = temp1;
        }

        u[k][k] = a[k][k];

        for(int i=k ; i<n ; i++)
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

    printMatrix((double *)a, n, "Target_out");
    printMatrix((double *)u, n, "Upper_out");
    printMatrix((double *)l, n, "Lower_out");
    saveResidual((double *)a, n, 0);

    auto end = chrono::high_resolution_clock::now();
    auto time_taken = chrono::duration_cast<chrono::milliseconds>(end - start);

    printf("\n\nTime taken: %.02fs\n", (float)time_taken.count()/1000);

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


void saveResidual(double *matrix, int dim, int id)
{
    // Saved as transpose
    ofstream outfile;
    outfile.open("seq_residual_" + to_string(id) + ".txt");

    for(int j=0 ; j<dim ; j++)
    {
        for(int i=0 ; i<dim ; i++)
        {
            outfile << fixed << setprecision(15) << matrix[i*dim + j] << " ";
        }
        outfile << '\n';
    }
    outfile.close();
}
