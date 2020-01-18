#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>

using namespace std;

int main(int argc, char *argv[])
{
    // Command line args: <size of matrix> <filename>

    string n_str(argv[1]);
    int n = stoi(n_str);                        // Dimension of the square matrix
    
    string filename = argv[2];
    ifstream infile;
    infile.open(filename);

    double** a;
    a = (double**)malloc(sizeof(double*)*(n));
    
    double* aarr;
    for(int i=0 ; i<n ; i++)
    {
        aarr = (double*)malloc(sizeof(double)*(n));
        for(int j=0 ; j<n ; j++)
        {
            infile >> aarr[j];
        }
        a[i] = aarr;
    }
    infile.close();

    double l21 = 0.0;

    for(int j=0 ; j<n ; j++)
    {
        double temp = 0.0;
        for(int i=0 ; i<n ; i++)
        {
            temp += a[i][j] * a[i][j];
        }
        l21 += sqrt(temp);
    }

    cout << "\nL2,1 Norm of the matrix = " << l21 << "\n";

    return 0;   
}