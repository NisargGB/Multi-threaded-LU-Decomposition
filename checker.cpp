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

    double res[n][n];
    double l21 = 0.0;

    for(int j=0 ; j<n ; j++)
    {
        double temp = 0.0;
        for(int i=0 ; i<n ; i++)
        {
            infile >> res[i][j];
            temp += res[i][j] * res[i][j];
        }
        l21 += sqrt(temp);
    }

    cout << "\nL2,1 Norm of the matrix = " << l21 << "\n";

    return 0;   
}