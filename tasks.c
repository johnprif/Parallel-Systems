/* Serial program for matrix-matrix product.
 *
 * VVD
 */
//Joanis Prifti 3321
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 2048
#define Afile "Amat2048"
#define Bfile "Bmat2048"
#define Cfile "Cmat2048"

int A[N][N], B[N][N], C[N][N];
int readmat(char *fname, int *mat, int n), 
    writemat(char *fname, int *mat, int n);

int main(int argc, char **argv)
{
	int i, j, k, sum, S, ii, jj;
    double start, end; //for times
    S=atoi(argv[1]); //take user input and convert to integer
	/* Read A & B matrices from files
	 */
	if (readmat(Afile, (int *) A, N) < 0) 
		exit( 1 + printf("file problem\n") );
	if (readmat(Bfile, (int *) B, N) < 0) 
		exit( 1 + printf("file problem\n") );\

    start=omp_get_wtime(); //get start time
    #pragma omp parallel num_threads(4) default(none) private(i,j) shared(A,B,C,S) reduction(+:sum)
    {
		#pragma omp single nowait
		{
			for (i = 0; i < N; i+=S)
    		{
				for (j = 0; j < N; j+=S)
				{
    		    	#pragma omp task default(none) firstprivate(i,j) private(ii,jj,k,sum) shared(A,B,C,S)
					{
						for(int ii=i; ii<S+i; ii++)
						{
							for(int jj=j; jj<S+j; jj++)
							{
								for(int k=sum=0; k<N; k++)
								{
									sum += A[ii][k]*B[k][jj];
								}
								C[ii][jj] = sum;
							}
						}
					}
				}
    		}
		}
    }
    end=omp_get_wtime(); //get end time
	/* Save result in file
	 */
	writemat(Cfile, (int *) C, N);
    printf("The time is: %f sec\nfor tasks=%d\n", end-start, S);
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