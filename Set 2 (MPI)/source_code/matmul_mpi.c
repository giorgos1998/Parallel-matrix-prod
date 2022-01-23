/*
 * Papadatos Georgios
 * AM: 3306
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1024
#define Afile "Amat1024.txt"
#define Bfile "Bmat1024.txt"
#define Cfile "Cmat1024.txt"

int A[N][N], B[N][N], C[N][N];
int readmat(char *fname, int *mat, int n), 
    writemat(char *fname, int *mat, int n);

int main(int argc, char *argv[])
{
	int i, j, k, sum, myID, nproc;
	double startTime, endTime, startCommTime, endCommTime;
	double commTime = 0.0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myID);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	

	// initialization
	if (myID == 0)
	{
		//printf("P%d, reading files\n", myID);

		/* Read A & B matrices from files
		 */
		if (readmat(Afile, (int *) A, N) < 0) 
			exit( 1 + printf("file problem\n") );
		if (readmat(Bfile, (int *) B, N) < 0) 
			exit( 1 + printf("file problem\n") );

		// start runtime timer
		startTime = MPI_Wtime();
	}

	// allocate matrices for each proccess
	int work = N / nproc;
	int myA[work][N];
	int myC[work][N];

	// start communications timer
	if (myID == 0)
	{
		startCommTime = MPI_Wtime();
	}

	// scatter matrix A and send whole matrix B
	MPI_Scatter(A, work*N, MPI_INT, myA, work*N, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(B, N*N, MPI_INT, 0, MPI_COMM_WORLD);

	// stop communications timer and add it to the total time
	if (myID == 0)
	{
		endCommTime = MPI_Wtime();
		commTime += endCommTime - startCommTime;
	}	

	// calculate part of the multiplication
	for(i = 0; i < work; i++)
		for (j = 0; j < N; j++)
		{
			for (k = sum = 0; k < N; k++)
				sum += myA[i][k] * B[k][j];
			myC[i][j] = sum;
		};
	
	// start communications timer
	if (myID == 0)
	{
		startCommTime = MPI_Wtime();
	}

	// gather all the multiplication parts
	MPI_Gather(myC, work*N, MPI_INT, C, work*N, MPI_INT, 0, MPI_COMM_WORLD);
	
	if (myID == 0)
	{
		// stop communications timer and get total communications time
		endCommTime = MPI_Wtime();
		commTime += endCommTime - startCommTime;

		// stop runtime timer and get result
		endTime = MPI_Wtime();
		printf("Execution time: %f seconds\n", endTime - startTime);
		printf("Communication time: %f seconds\n", commTime);
		
		/* Save result in file
		 */
		writemat(Cfile, (int *) C, N);	
	}
	
	MPI_Finalize();
	return (0);
}


/* Utilities to read & write matrices from/to files
 * VVD
 */

#define _mat(i,j) (mat[(i)*n + (j)])


int readmat(char *fname, int *mat, int n)
{
	FILE *fp;
	int  i, j;
	
	if ((fp = fopen(fname, "r")) == NULL)
		return (-1);
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (fscanf(fp, "%d", &_mat(i,j)) == EOF)
			{
				fclose(fp);
				return (-1); 
			};
	fclose(fp);
	return (0);
}


int writemat(char *fname, int *mat, int n)
{
	FILE *fp;
	int  i, j;
	
	if ((fp = fopen(fname, "w")) == NULL)
		return (-1);
	for (i = 0; i < n; i++, fprintf(fp, "\n"))
		for (j = 0; j < n; j++)
			fprintf(fp, " %d", _mat(i, j));
	fclose(fp);
	return (0);
}
