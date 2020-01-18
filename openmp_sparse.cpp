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

using namespace std;

// #define RAND_MAX 1000

void printMatrix(double** matrix, int dim, string msg);
void saveResidual(double** matrix, int dim, int id);
// random_device()

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
                                               // Matrices A, L, U
    double** a;
    a = (double**)malloc(sizeof(double*)*(n));
    double** l;
    l = (double**)malloc(sizeof(double*)*(n));
    double** u;
    u = (double**)malloc(sizeof(double*)*(n));
    
    //int pi[n];                                  // Compact permutation matrix
    int* pi;
    pi = (int*)malloc(sizeof(int)*(n));

    srand(time(0));

    // Initializations
    double* aarr;
    double* larr;
    double* uarr;
    for (int i=0 ; i<n ; i++)
    {
        pi[i] = i;
        aarr = (double*)malloc(sizeof(double)*(n));
        larr = (double*)malloc(sizeof(double)*(n));
        uarr = (double*)malloc(sizeof(double)*(n));
        for (int j=0 ; j<n ; j++)
        {
            aarr[j] = ((double)(rand()%1000)) / 100.0;
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
    auto start2 = chrono::high_resolution_clock::now();

    // printMatrix((double *)a, n, "Target");
    // printMatrix((double *)u, n, "Upper");
    // printMatrix((double *)l, n, "Lower");
    
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
            return 1;
        }

        int temp0;
        temp0 = pi[k];
        pi[k] = pi[kd];
        pi[kd] = temp0;

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
    auto time_taken2 = chrono::duration_cast<chrono::milliseconds>(start2 - start);
    printf("\nTime taken: %.02fs, %.02fs\n", (float)time_taken.count()/1000, (float)time_taken2.count()/1000);

    // saveResidual(a, n, 1);
    return 0;
}


void printMatrix(double** matrix, int dim, string msg)
{
    cout << '\n' << msg << '\n';
    for(int i=0 ; i<dim ; i++)
    {
        for(int j=0 ; j<dim ; j++)
        {
            printf("%.02lf ", matrix[i][j]);
        }
        printf("\n");
    }
}


void saveResidual(double** matrix, int dim, int id)
{
    // Saved as transpose
    ofstream outfile;
    outfile.open("residual_" + to_string(id) + ".txt");

    for(int j=0 ; j<dim ; j++)
    {
        for(int i=0 ; i<dim ; i++)
        {
            outfile << fixed << setprecision(15) << matrix[i][j] << " ";
        }
        outfile << '\n';
    }
    outfile.close();
}