The syntax for various types of operations-

For Convolution without padding : $./COP_Task1_Sub1 ValidConvolution Matrix_filename.txt matrix_size Kernel_filename.txt kernel_size

For Convolution with padding(same size) : $./COP_Task1_Sub1 SameConvolution Matrix_filename.txt matrix_size Kernel_filename.txt kernel_size

For convolution by matrix multiplication method : $./COP_Task1_Sub1 MatrixMultiplication Matrix_filename.txt matrix_size Kernel_filename.txt kernel_size mode(Default or OpenBLAS or IntelMKL or Pthreaded)

For convolution by matrix multiplication method of same size : $./COP_Task1_Sub1 MatrixMultiplicationSameSize Matrix_filename.txt matrix_size Kernel_filename.txt kernel_size

For RELU activation of matrix : $./COP_Task1_Sub1 RELU Matrix_filename.txt matrix_size

For tanh activation of matrix : $./COP_Task1_Sub1 tanh Matrix_filename.txt matrix_size

For subsampling of square input matrices of any size with max pooling function : $./COP_Task1_Sub1 MaxPool Matrix_filename.txt matrix_size

For subsampling of square input matrices of any size with average pooling function : $./COP_Task1_Sub1 AveragePool Matrix_filename.txt matrix_size

For converting a vector of random floats to a vector of probabilities with softmax function : $./COP_Task1_Sub1 Softmax Vector_filename.txt

For converting a vector of random floats to a vector of probabilities with sigmoid function : $./COP_Task1_Sub1 Sigmoid Vector_filename.txt

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Extra Points:
-> All functions are present in "functions.h" which is included as a library file in all the .cpp files.

-> Makefile consists of all the .cpp files along with their dependencies and compilation command arguments.

-> In main.cpp we take in the input from command line arguments and then read the file with the given name for matrix, kernel, and vector elements values. Then, the corresponding function is called inside main and output is written in appropriate files (‘operation name’-result.txt)

-> For maxpool and average pool, a 2X2 Pool is used having a stride of 2 avoiding any overlapping regions.

-> For same convolution (i.e. convolution with same size), the value of (kernel size - 1)/2 is calculated first and then direct convolution and matrix multiplication is performed when kernel size is odd.
When kernel size is even, it requires uneven handling. We have padded 
(kernel size -2)/2 on the left and top side while padding (kernel size/2) on the right and bottom side.

-> If there is any mistake in writing the command line argument, the program terminates with an appropriate error message.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

-> The functions (in separate .cpp files) which correspond to their respective operations have the format of input parameters in the code as mentioned below-
std::vector<std::vector<float> > sameconvolution(matrix, kernel , resulting matrix size, input matrix size, kernel size);
	Called when convolution without size shrinking is required.

std::vector<std::vector<float> > validconvolution(matrix, kernel , resulting matrix size, input matrix size, kernel size);
	Called when convolution with size shrinking is required.

float* averagepool(matrix , matrix size);
	Called when average pooling is to be done on a matrix.

float* convertedmatrix(input matrix, matrix size , kernel size );
	Called when conversion of a normal matrix is to be done to a Toeplitz matrix.

float* matrixmult(Toeplitz matrix, kernel, matrix size, kernel size, mode);
	Called when called when convolution (reduced size) is to be done using matrix
	multiplication in default method or by OpenBLAS implemenation (set by mode). It takes the Toeplitz form of the input matrix as its one of the
	parameter. The other being the kernel matrix.

float* matrixmult2(Toeplitz matrix, kernel, matrix size, kernel size );
	Called when called when convolution (reduced size) is to be done using matrix
	multiplication with Intel MKL implementation. It takes the Toeplitz form of the input matrix as its one of the
	parameter. The other being the kernel matrix.


float* samemult(Toeplitz matrix, kernel, matrix size, kernel size );
	Called when called when convolution (same size) is to be done using matrix
	multiplication. It takes the Toeplitz form of the input matrix as its one of the
	parameter. The other being the kernel matrix. It makes the padded matrix and then calls convertedmatrix and matrixmult functions.

float* maxpool(matrix , matrix size);
	Called when maxpool operation is to be done on an input matrix. Parameters: 
	Matrix and matrix size.

float* relu(input matrix, matrix size);
	Called when relu operation is to be done on an input matrix.

std::vector<float> sigmoid(input vector);
	Called when sigmoid operation is to be done on an input vector.

std::vector<float> softmax(input vector);
	Called when softmax operation is to be done on an input vector.

float* tanh(input matrix, matrix size);
	Called when tanh operation is to be done on an input matrix.
