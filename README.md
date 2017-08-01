# Prim Minimum Spanning Tree using MPI


In this program,  Prim MST is implemented using MPI.

jobscript2 and jobscript4 and jobscript8 include 2 and 4 and 8 processors.

The path for files is set to "/MST/" inside the code, and the mpicc address is set to "/wsu/apps/gnu-4.4.7/openmpi/openmpi-1.8.8/bin/mpicc".

Matrix.txt includes the size of vertices plus the adjacncy matrix. Here I used 1000 as infinity number. the adjacancy matrix is only upper triangle
if Matrix,.txt only includes the size of vertices, then the algorithm generates the adjacancy matrix randomly.
I put  Matrix.txt which includes the example of the book.
If you want to run the algorithm for 128 vertices, then you have to replace the previous file with the Matrix.txt which is in folder Matrix128.
(I should notice that the second file includes a number 128 and newline after it. If you are using your own file please make sure there is a newline after number 128)

Results.txt includes the result of seleced edges after prime algorithm and the total weight.

Time.txt includes the running time of algorithm.

The Time.txt includes below values at the end:

/*********************************************/

 Number of processors: 2

 Number of vertices: 128

 Time of execution: 0.003699

 Total Weight: 184

/*********************************************/

 Number of processors: 4

 Number of vertices: 128

 Time of execution: 0.019089

 Total Weight: 184

/*********************************************/

 Number of processors: 8

 Number of vertices: 128

 Time of execution: 0.034099

 Total Weight: 184
 
/*********************************************/ 

The total weight might be different because the matrix is random

The execution time for two prcessors is less than excecution time for 4 or 8 processors

the execution time for 1 processor is :

 Number of processors: 1

 Number of vertices: 128

 Time of execution: 0.005692

 Total Weight: 184


So the speedup for 2 processors is : 0.005692 / 0.003699 = 1.5

So the speedup for 4 processors is : 0.005692 / 0.019089 = 0.3

So the speedup for 8 processors is : 0.005692 / 0.034099 = 0.166


