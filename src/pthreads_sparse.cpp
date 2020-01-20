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
#include "../include/functions.h"

using namespace std;

int t_num = 0;
int dim;
double **a, **l, **u, **pi;
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
//     int low = k + t_num*(dim-k)/t;
//     int high =  k + (t_num+1)*(dim-k)/t;
//     t_num++;
//     pthread_exit((void*)thread_function(arg, low, high));
// }

void* thread_function(void* arg)
{
    arguments* arg_struct = (arguments*)arg;
    int k = arg_struct->k_value;
    int low = arg_struct->low_value;
    int high = arg_struct->high_value;

    for(int i = low; i < min(high, dim); i++)
    {
        for(int j=k ; j<dim ; j++)
        {
            a[i][j] -= l[i][k] * u[k][j];// minus equal to
        }
    }
    pthread_exit(NULL);
    // cout << "thread id is: " << arg_struct->thread_id << endl;
}

// uniform_real_distribution <double> dist (0, 10);
// random_device rd;

vector<double**> pthreadsDecomposition(vector<double**> input, int n_in, int t)
{
    a = input[0];
    l = input[1];
    u = input[2];
    pi = input[3];
    dim = n_in;


    // LU Decomposition algorithm
    for(int k=0 ; k<dim ; k++)
    {
        // cout << "value of k is: " << k << endl;
        double max = 0.0;
        int kd = -1;
        for(int i=k; i<dim ; i++)
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

        //#pragma omp parallel sections
        {
            //#pragma omp section
            {
                double temp1;//swap a[k,:] and a[kd,:]
                //#pragma omp parallel for num_threads(t) default(none) private(temp1) shared(n,a,k,kd,t,l) 
                for(int i=0 ; i<dim ; i++)
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
        for(int i=k+1 ; i<dim ; i++)
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
            arg_struct->low_value = k + t_num*(dim-k)/t;
            arg_struct->high_value = k + (t_num + 1)*(dim-k)/t;
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

    return {a, l, u, pi};
}
