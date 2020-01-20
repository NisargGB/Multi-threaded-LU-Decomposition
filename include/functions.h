#ifndef FUNCTIONS_INCLUDE
#define FUNCTIONS_INCLUDE

#include <string>
#include <vector>

void printInputError();
std::vector<double**> initialiseMatrices(int n);
void printMatrix(double** matrix, int dim, std::string msg);
void saveMatrix(double** matrix, int dim, std::string filename, std::string mode);
std::vector<double**> initialiseMatrices(int n);
std::vector<double**> sequentialDecomposition(std::vector<double**> input, int n);
std::vector<double**> openmpDecomposition(std::vector<double**> input, int n, int t);
std::vector<double**> pthreadsDecomposition(std::vector<double**> input, int n, int t);
double l21norm(double** matrix, int n);

#endif
