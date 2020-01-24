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
#define EPSILON 1e-9

int t_num = 0;
int dim;
double *a, *l, *u, *pi;
void* thread_function(void* arg);

//struct for passing the arguments to the thread of "Double For" loop
struct arguments
{
    int low_value;
    int high_value;
    int k_value;
    int thread_id;
};

//struct for passing the arguments to the thread of swap_a and swap_l
struct swap_arguments
{
    int k_value;
    int kd_value;
};

//this function is executed by every thread of "Double For" loop; the thread updates the matrix a from row indices "low" to "high" and then calls exit
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
            a[i*dim + j] -= l[i*dim + k] * u[k*dim + j];// minus equal to
        }
    }
    pthread_exit(NULL);
}

//this function is executed by thread for swapping the kth and the kdth rows of matrix "a",  
void* swap_a(void* arg)
{
    double temp1;//swap a[k,:] and a[kd,:]
    swap_arguments* swap_struct = (swap_arguments*)arg;
    int k = swap_struct->k_value;
    int kd = swap_struct->kd_value;
    for(int i=0 ; i<dim ; i++)
    {
        temp1 = a[k*dim + i];
        a[k*dim + i] = a[kd*dim + i];
        a[kd*dim + i] = temp1;
    }
    pthread_exit(NULL);
}

//this function is executed by thread for swapping the kth and the kdth rows of matrix "l" from columns 0 to k-1,
void* swap_l(void* arg)
{
    double temp2;
    swap_arguments* swap_struct = (swap_arguments*)arg;
    int k = swap_struct->k_value;
    int kd = swap_struct->kd_value;
    for(int i=0 ; i<k ; i++)
    {
        temp2 = l[k*dim + i];
        l[k*dim + i] = l[kd*dim + i];
        l[kd*dim + i] = temp2;
    }
    pthread_exit(NULL);
}

//this function takes the matrices as a contiguious array, dimension of matrix and number of threads as input and uses pthreads to calculate the "l" and the "u" matrices
vector<double*> pthreadsDecompositionDense(vector<double*> input, int n_in, int t)
{
    a = input[0];
    l = input[1];
    u = input[2];
    pi = input[3];
    dim = n_in;

    for(int k=0 ; k<dim ; k++)          //the outer for loop which updates the kth row and kth column of the matrix a.
    {
        double max = 0.0;
        int kd = -1;
        for(int i=k; i<dim ; i++)       //for calculating the max value's index kd
        {
            if(max < abs(a[i*dim + k]))
            {
                max = abs(a[i*dim + k]);
                kd = i;
            }
        }
        
        if(max < EPSILON)               // since we are doing double comparisons so instead of checking max = 0.0, we did max < EPSILON defined above as 1e-9
        {
            printf("\n\nSingular matrix ERROR\nProgram terminated with code 1\n\n");
            // cout << k << '\n';
            return {};
        }

        //updating the pi vector to store the permutation matrix P
        int temp0;
        temp0 = pi[k];
        pi[k] = pi[kd];
        pi[kd] = temp0;

        //swap a[k,:] and a[kd,:]
        double temp1;
        for(int i=0 ; i<dim ; i++)
        {
            temp1 = a[k*dim + i];
            a[k*dim + i] = a[kd*dim + i];
            a[kd*dim + i] = temp1;
        }
        //swap l[k,1:k-1] and l[kd,1:k-1]
        double temp2;
        for(int i=0 ; i<k ; i++)
        {
            temp2 = l[k*dim + i];
            l[k*dim + i] = l[kd*dim + i];
            l[kd*dim + i] = temp2;
        }

        {
            // // for doing swap a and swap l in parallel; it creates two threads and pass one to swap_a and one to swap_l function
            // pthread_t swap_threads[2];
            // swap_arguments *swap_struct;
            // swap_struct = (swap_arguments*)malloc(sizeof(swap_arguments));
            // swap_struct->k_value = k;
            // swap_struct->kd_value = kd;
            // pthread_create(&swap_threads[0], NULL, swap_a, (void*)swap_struct);
            // pthread_create(&swap_threads[1], NULL, swap_l, (void*)swap_struct);
            // // waits here for both the threads to finish their tasks and then proceed further into the algorithm
            // void* swap_status[2];
            // pthread_join(swap_threads[0], &swap_status[0]);
            // pthread_join(swap_threads[1], &swap_status[1]);
        }

        u[k*dim + k] = a[k*dim + k];

        for(int i=k+1 ; i<dim ; i++)
        {
            l[i*dim + k] = a[i*dim + k] / u[k*dim + k];
            u[k*dim + i] = a[k*dim + i];
        }

        t_num = 0;//initialise t_num back from zero
        // create t threads; divide the work among the threads uniformly (following a static schedule) by passing the "low" and the "high" row indices to every thread
        // and then call the thread_function to do the "a" matrix update
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

        // waits here for all the t threads to finish their tasks and then proceed further into the algorithm; the synchronization step 
        void* status[t];
        for(int i = 0; i < t; i++)
        {
            // cout << "thread joining" << endl;
            pthread_join(threads[i],&status[i]);
        }

    }

    return {a, l, u, pi};
}