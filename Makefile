objects = main2.o main.o sameconvolution.o validconvolution.o matrix_mult.o relu.o tanh.o \
					maxpool.o averagepool.o sigmoid.o softmax.o
CC = g++
CFLAGS = -Wall -c

edit : $(objects)
						$(CC) $(objects) -o COP_Task1_Sub1 -lpthread

main.o : main.cpp functions.h
						$(CC) $(CFLAGS) main.cpp
main2.o : main2.cpp functions.h
						$(CC) $(CFLAGS) main2.cpp
sameconvolution.o : sameconvolution.cpp functions.h
						$(CC) $(CFLAGS) sameconvolution.cpp
validconvolution.o : validconvolution.cpp functions.h
						$(CC) $(CFLAGS) validconvolution.cpp
matrix_mult.o : matrix_mult.cpp functions.h
						$(CC) $(CFLAGS) matrix_mult.cpp
relu.o : relu.cpp functions.h
						$(CC) $(CFLAGS) relu.cpp
tanh.o : tanh.cpp functions.h
						$(CC) $(CFLAGS) tanh.cpp
sigmoid.o : sigmoid.cpp functions.h
						$(CC) $(CFLAGS) sigmoid.cpp
maxpool.o : maxpool.cpp functions.h
						$(CC) $(CFLAGS) maxpool.cpp
averagepool.o : averagepool.cpp functions.h
						$(CC) $(CFLAGS) averagepool.cpp
softmax.o : softmax.cpp functions.h
						$(CC) $(CFLAGS) softmax.cpp

clean :
						rm edit $(objects)
