#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>

// nomatrix =1 means the matrix will initialized randomly
//nomatrix=0 means read matrix from file Matrix.txt
int nomatrix ;
int size;  // number of processors
int rank;  // rank of current processor
int* MatrixChunk; // chunk of matrix belong to each processor
int mSize; // size os matrix or number of vertices
int* displs; // displs specifies the displacement 
int* sendcounts; // sendcounts  specifying the number of elements to send to each processor 
typedef struct { int v1; int v2; } Edge;
int* MST; //minimum spanning tree including the selected edges
int minWeight; // min weight
FILE *f_matrix; // file of matrix
FILE *f_time; // time result file
FILE *f_result; // file of selected edges
 

int main(int argc,char *argv[])
{

  MPI_Init ( &argc, &argv );
  MPI_Comm_rank ( MPI_COMM_WORLD, &rank);
  MPI_Comm_size ( MPI_COMM_WORLD, &size );
  printf("rank is %d and number of processors is %d\n", rank , size);

  srand(time(NULL) + rank);
  if (rank==0)
  {
    f_matrix = fopen("/wsu/home/fp/fp06/fp0634/hw6/Matrix.txt", "r");
	if (f_matrix){
		fscanf(f_matrix, "%d\n", &mSize);
	}
	if (feof(f_matrix))
		nomatrix = 1 ; // there is no matrix to read from file
	else 
		nomatrix = 0 ;
	printf("msize is %d, and nomatrix is %d\n", mSize, nomatrix);
  }

  MPI_Bcast(&mSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

  /************************************************/
  int i,j,k;       

  displs = (int*)malloc(sizeof(int) * size);   
  sendcounts = (int*)malloc(sizeof(int) * size);

  displs[0] = 0;
  sendcounts[0] = mSize / size;
  int remains = size - (mSize % size); // if number of vertices is not a multiply of number of processors
  for (i = 1; i < remains; ++i) 
  {
    sendcounts[i] = sendcounts[0];
    displs[i] = displs[i - 1] + sendcounts[i - 1];
  }
  for (i = remains; i < size; ++i)
  {
    sendcounts[i] = sendcounts[0] + 1;
    displs[i] = displs[i - 1] + sendcounts[i - 1];
  }

  
  int* matrix;
  if (rank==0) // rank0 reads the file of Matrix.txt
  {
    matrix = (int*)malloc(mSize*mSize*sizeof(int));
    for (i = 0; i < mSize; ++i )
    {
	  matrix[mSize*i+j] = 0 ;
      for(j = i + 1; j < mSize; ++j )
      {
        int buf;
        if (!nomatrix) { // read from file the upper triangle
			fscanf(f_matrix, "%d\n", &buf);
			matrix[mSize*i+j] = matrix[mSize*i+j] = buf ;
		}
		else { // or initialize randomly
			matrix[mSize*i+j] = matrix[mSize*i+j] = rand() % 20;
		}
		printf("matrix(%d,%d) is %d\n", i ,j , matrix[mSize*i+j] );
      }
    }
    fclose(f_matrix);
  }
  MatrixChunk = (int*)malloc(sendcounts[rank]*mSize*sizeof(int));
  //after this each processor needs its own chunk of data

  
  MPI_Datatype matrixString; 
  MPI_Type_contiguous(mSize,MPI_INT, &matrixString);
  MPI_Type_commit(&matrixString);
  //here the chunk each processor needs will be scatter to it
  MPI_Scatterv(matrix,sendcounts,displs,matrixString,MatrixChunk,sendcounts[rank],matrixString,0,MPI_COMM_WORLD);
  if(rank==0)
	// rank0 is writing its own chunk on screen
  {
    for (i = 0; i < mSize; ++i)
    {  
      for ( j = 0; j < mSize; ++j)
      {
        printf("%d ", MatrixChunk[mSize*i+j]); // just for test
      }
      printf("\n");
    }
  }
  
  MST = (int*)malloc(sizeof(int)*mSize); // max size is number of vertices
  for ( i = 0; i < mSize; ++i)
  {
    MST[i] = -1;
  }
  
  double start; 

  start = MPI_Wtime(); // start to calculate running time
      
  MST[0] = 0;
  minWeight = 0;

  int min;
  int v1 = 0;
  int v2 = 0;

  struct { int min; int rank; } minRow, row;
  Edge edge;
  for ( k = 0; k < mSize - 1; ++k)
  {
    min = INT_MAX;
    for ( i = 0; i < sendcounts[rank]; ++i)
    {
      if (MST[i + displs[rank]] != -1) 
      {
        for ( j = 0; j < mSize; ++j)
        {
          if (MST[j] == -1) 
          {
            // if the MatrixChunk[mSize*i+j] is less than min value
            if ( MatrixChunk[mSize*i+j] < min && MatrixChunk[mSize*i+j] != 0)
            {
              min = MatrixChunk[mSize*i+j];
              v2 = j; // change the current edge
              v1 = i;   
            }
          }
        }
      }
    }
    row.min = min;
    row.rank = rank; // the rank of min in row
	// each proc have to send the min row to others 
    MPI_Allreduce(&row, &minRow, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD); 
    edge.v1 = v1 + displs[rank];
    edge.v2 = v2;
    MPI_Bcast(&edge, 1, MPI_2INT, minRow.rank, MPI_COMM_WORLD);

    MST[edge.v2] = edge.v1;
    minWeight += minRow.min;
  }
  /**************************************************************/
  double finish, calc_time; 
  finish = MPI_Wtime();
  calc_time = finish-start;

  if (rank == 0) // again rank 0 will writte its own process time 
  {
    f_result = fopen("/wsu/home/fp/fp06/fp0634/hw6/Result.txt", "w");
    fprintf(f_result,"The min minWeight is %d\n ", minWeight);
    for ( i=0; i< mSize; ++i)
    {
      fprintf(f_result,"Edge %d %d\n",i, MST[i]);
    }
    fclose(f_result);

    f_time = fopen("/wsu/home/fp/fp06/fp0634/hw6/Time.txt", "a+");
    fprintf(f_time, "\n Number of processors: %d\n Number of vertices: %d\n Time of execution: %f\n Total Weight: %d\n\n", size, mSize ,calc_time, minWeight);
    fclose(f_time);
    printf("\n Number of processors: %d\n Number of vertices: %d\n Time of execution: %f\n Total Weight: %d\n\n", size, mSize ,calc_time, minWeight);
  }

  MPI_Finalize();
  return 0;

  }