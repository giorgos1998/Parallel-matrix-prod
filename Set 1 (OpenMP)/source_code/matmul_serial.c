/*
 * Papadatos Georgios
 * AM: 3306
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

#define N 2048
#define Afile "Amat2048.txt"
#define Bfile "Bmat2048.txt"
#define Cfile "new_Cmat2048.txt"

int A[N][N], B[N][N], C[N][N];
int readmat(char *fname, int *mat, int n), 
    writemat(char *fname, int *mat, int n);

int main()
{
	int i, j, k, sum;

	/* Read A & B matrices from files
	 */
	if (readmat(Afile, (int *) A, N) < 0) 
		exit( 1 + printf("file problem\n") );
	if (readmat(Bfile, (int *) B, N) < 0) 
		exit( 1 + printf("file problem\n") );

	// Start timer
	double start = omp_get_wtime();
	
	
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			for (k = sum = 0; k < N; k++)
				sum += A[i][k]*B[k][j];
			C[i][j] = sum;
		};
			
	// Stop timer and get result
	double time = omp_get_wtime() - start;
	printf("execution time: %f\n", time);

	/* Save result in file
	 */
	writemat(Cfile, (int *) C, N);

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
