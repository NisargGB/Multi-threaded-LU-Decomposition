#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>
#include <chrono>
#include <fstream>
#include <omp.h>
#include <math.h>

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
            //a[i][j] = 3*(i) + (j+1);
            // a[i][j] = rand()%10;
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


    //printMatrix((double *)a, n, "Target");
    //printMatrix((double *)u, n, "Upper");
    //printMatrix((double *)l, n, "Lower");
    
    // Sequential Algorithm

    for(int k=0 ; k<n ; k++)
    {
        //cout << k << endl;
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

        #pragma omp parallel sections
        {
            #pragma omp section
            {
                double temp1;//swap a[k,:] and a[kd,:]
                # pragma omp parallel for num_threads(t/2) default(none) private(temp1) shared(n,a,k,kd)
                for(int i=0 ; i<n ; i++)
                {
                    temp1 = a[k*n + i];
                    a[k*n + i] = a[kd*n + i];
                    a[kd*n + i] = temp1;
                }
            }
            //swap l[k,1:k-1] and l[kd,1:k-1]
            #pragma omp section
            {
                double temp2;
                # pragma omp parallel for num_threads(t/2) default(none) private(temp2) shared(k,l,kd,n)
                for(int i=0 ; i<k ; i++)
                {
                    temp2 = l[k*n + i];
                    l[k*n + i] = l[kd*n + i];
                    l[kd*n + i] = temp2;
                }
            }
        }

        u[k*n + k] = a[k*n + k];

        # pragma omp parallel for num_threads(t) default(none) shared(k,l,a,u,n)
        for(int i=k+1 ; i<n ; i++)
        {
            l[i*n + k] = a[i*n + k] / u[k*n + k];
            u[k*n + i] = a[k*n + i];
        }

        int thread_sqrt = floor(sqrt(t));
        //cout << thread_sqrt << endl;
        # pragma omp parallel for num_threads(thread_sqrt) default(none) shared(k,l,a,u,n,thread_sqrt)
        for(int i=k ; i<n ; i++)
        {
            # pragma omp parallel for num_threads(thread_sqrt) default(none) shared(k,l,a,u,n,i,thread_sqrt)
            for(int j=k ; j<n ; j++)
            {
                a[i*n + j] -= l[i*n + k] * u[k*n + j];// minus equal to
            }
        }
    }

    //printMatrix(a, n, "Residual_matrix");
    //printMatrix(u, n, "Upper_out");
    //printMatrix(l, n, "Lower_out");
    saveResidual(a, n, 0);
    //cout << "PI vector" << endl;
    for(int m = 0; m < n; m++)
    {
        //cout << pi[m] << " ";
    }

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
    outfile.open("par_residual_" + to_string(id) + ".txt");

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
