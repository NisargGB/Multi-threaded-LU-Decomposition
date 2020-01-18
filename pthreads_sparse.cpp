#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>
#include <chrono>
#include <fstream>
#include <omp.h>
#include <math.h>
#include <random>
#include <pthread.h>

using namespace std;

// #define RAND_MAX 1000
double** a;
double** l;
double** u;
int n;
int t_num = 0;
int t;
void printMatrix(double** matrix, int dim, string msg);
void saveResidual(double** matrix, int dim, string filename);
void* thread_function(void* arg);

struct arguments
{
    int low_value;
    int high_value;
    int k_value;
    int thread_id;
};

// void* thread_function2(void* arg)
// {
//     int thread_number = t_num;
//     cout << t_num << endl;
//     int k = (int)arg;
//     int low = k + t_num*(n-k)/t;
//     int high =  k + (t_num+1)*(n-k)/t;
//     t_num++;
//     pthread_exit((void*)thread_function(arg, low, high));
// }

void* thread_function(void* arg)
{
    arguments* arg_struct = (arguments*)arg;
    int k = arg_struct->k_value;
    int low = arg_struct->low_value;
    int high = arg_struct->high_value;

    for(int i = low; i < min(high, n); i++)
    {
        for(int j=k ; j<n ; j++)
        {
            a[i][j] -= l[i][k] * u[k][j];// minus equal to
        }
    }
    pthread_exit(NULL);
    // cout << "thread id is: " << arg_struct->thread_id << endl;
}

// uniform_real_distribution <double> dist (0, 10);
// random_device rd;

int main(int argc, char *argv[])
{
    // Command line args: <size of matrix A> <number of threads>
    auto start = chrono::high_resolution_clock::now();

    string n_str(argv[1]);
    n = stoi(n_str);                        // Dimension of the square matrix
    string t_str(argv[2]);
    t = stoi(t_str);                        // Number of threads

    // Range of random numbers: TODO
    int range = 10;
    long factor = RAND_MAX / range;
                                           // Matrices A, L, U
    a = (double**)malloc(sizeof(double*)*(n));
    l = (double**)malloc(sizeof(double*)*(n));
    u = (double**)malloc(sizeof(double*)*(n));
    
    //int pi[n];                                  // Compact permutation matrix
    int* pi;
    pi = (int*)malloc(sizeof(int)*(n));

    srand(time(0));

    // Initializations
    double* aarr;
    double* larr;
    double* uarr;
    // #pragma omp parallel for num_threads(t) default(none) shared(n, pi, a, l, u) collapse(2)
    for (int i=0 ; i<n ; i++)
    {
        pi[i] = i;
        aarr = (double*)malloc(sizeof(double)*(n));
        larr = (double*)malloc(sizeof(double)*(n));
        uarr = (double*)malloc(sizeof(double)*(n));
        for (int j=0 ; j<n ; j++)
        {
            // aarr[j] = ((double)(rand()%1000)) / 100.0;
            aarr[j] = 3*(i) + (j+1);
            // aarr[j] = rand()%10;
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

    // printMatrix(a, n, "Target");
    // printMatrix(u, n, "Upper");
    // printMatrix(l, n, "Lower");
    
    for(int k=0 ; k<n ; k++)
    {
        // cout << "value of k is: " << k << endl;
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
            //return 1;
        }

        int temp0;
        temp0 = pi[k];
        pi[k] = pi[kd];
        pi[kd] = temp0;

        //#pragma omp parallel sections
        {
            //#pragma omp section
            {
                double temp1;//swap a[k,:] and a[kd,:]
                //#pragma omp parallel for num_threads(t) default(none) private(temp1) shared(n,a,k,kd,t,l) 
                for(int i=0 ; i<n ; i++)
                {
                    temp1 = a[k][i];
                    a[k][i] = a[kd][i];
                    a[kd][i] = temp1;
                    // if(i < k)
                    // {
                    //     temp1 = l[k][i];
                    //     l[k][i] = l[kd][i];
                    //     l[kd][i] = temp1;
                    // }
                }
            }
            //swap l[k,1:k-1] and l[kd,1:k-1]
            //#pragma omp section
            {
                double temp2;
                //#pragma omp parallel for num_threads(t) default(none) private(temp2) shared(k,l,kd,n,t2)
                for(int i=0 ; i<k ; i++)
                {
                    temp2 = l[k][i];
                    l[k][i] = l[kd][i];
                    l[kd][i] = temp2;
                }
            }
        }

        u[k][k] = a[k][k];

        //#pragma omp parallel for num_threads(t) default(none) shared(k,l,a,u,n)
        for(int i=k+1 ; i<n ; i++)
        {
            l[i][k] = a[i][k] / u[k][k];
            u[k][i] = a[k][i];
        }

        t_num = 0;
        pthread_t threads[t];
        for(int i = 0; i < t; i++)
        {
            // cout << "thread creating" << endl;
            arguments *arg_struct;
            arg_struct = (arguments*)malloc(sizeof(arguments));
            arg_struct->low_value = k + t_num*(n-k)/t;
            arg_struct->high_value = k + (t_num + 1)*(n-k)/t;
            arg_struct->k_value = k;
            arg_struct->thread_id = t_num;

            pthread_create(&threads[i], NULL, thread_function, (void*)arg_struct);
            t_num++;
        }

        void* status[t];
        for(int i = 0; i < t; i++)
        {
            // cout << "thread joining" << endl;
            pthread_join(threads[i],&status[i]);
        }

        // tuple<double*, double*, double*>* tup = (tuple<double*, double*, double*>*)status[0];
        // a = (double* ) (get<0>(*tup));
        // l = (double* ) (get<1>(*tup));
        // u = (double* ) (get<2>(*tup));

    }

    // printMatrix(a, n, "Residual_matrix");
    // printMatrix(u, n, "Upper_out");
    // printMatrix(l, n, "Lower_out");
    //  cout << "PI vector" << endl;
    // for(int m = 0; m < n; m++)
    // {
    //     cout << pi[m] << " ";
    // }

    auto end = chrono::high_resolution_clock::now();
    auto time_taken = chrono::duration_cast<chrono::milliseconds>(end - start);

    printf("\nTime taken: %.02fs\n", (float)time_taken.count()/1000);
    saveResidual(a, n, "ALU.txt");
    saveResidual(l, n, "ALU.txt");
    saveResidual(u, n, "ALU.txt");
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


void saveResidual(double** matrix, int dim, string filename)
{
    ofstream outfile;
    outfile.open(filename, std::ios_base::app);

    for(int i=0 ; i<dim ; i++)
    {
        for(int j=0 ; j<dim ; j++)
        {
            outfile << fixed << setprecision(15) << matrix[i][j] << " ";
        }
        outfile << '\n';
    }
    outfile.close();
}