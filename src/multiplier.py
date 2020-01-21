#!/usr/bin/env python
# coding: utf-8

# In[8]:


import numpy
# from skfeature.utility.sparse_learning import calculate_l21_norm

# Run by: $python multiplier.py --n=3 --alu==<filename>


from optparse import OptionParser
parser = OptionParser()
parser.add_option("--n", dest="n", default=100)
parser.add_option("-f", dest="filename", default="")
parser.add_option("--alu", dest="myfile", default="../bin/ALU.txt")

(opts, args) = parser.parse_args()
n = int(opts.n)

def read(filename):
    lines = open(filename, 'r').read().splitlines()
    A = []
    Ares = []
    L = []
    U = []
    Pi = []
    matrix = A
    counter = 0;
    for line in lines:
#         print(line)
        if counter%n != 0 or counter == 0:
            matrix.append(list(map(float, line.split(" ")[:-1])))
            counter = counter + 1
        else:
            if(counter == n):
                matrix = Ares
            elif(counter == 2*n):
                matrix = L
            elif(counter == 3*n):
                matrix = U
            elif(counter == 4*n):
                matrix = Pi
            matrix.append(list(map(float, line.split(" ")[:-1])))
            counter = counter + 1
        if(counter == 5*n):
            break
    return A, Ares, L, U, Pi

def printMatrix(matrix):
    matrix = numpy.array(matrix)
    for line in matrix:
#         print ("\t".join(map(str,line)))
        print(line)

A, Ares, L, U, Pi = read(opts.myfile)
A = numpy.matrix(A)
P = numpy.zeros((n,n))
# Pi = numpy.matrix(Pi)
Ares = numpy.matrix(Ares)
L = numpy.matrix(L)
U = numpy.matrix(U)

for x in range(n):
    P[x][int(Pi[0][x])] = 1

C = L * U # easy and intuitive, isn't it?
D = P * A
ans = D - C
printMatrix(ans)
print(numpy.linalg.norm(ans))
# util.sparse_learning.calculate_l21_norm(ans)
# printMatrix(D)
# printMatrix(A)
# printMatrix(B)



# In[ ]:





# In[ ]:




