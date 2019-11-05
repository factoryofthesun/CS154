/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */

char transpose_submit_desc[] = "Transpose submission"; 
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	int i,j,d,v1,v2; 
	if (M == 32 && N == 32)
	{
		for(int f = 0; f < 4; f++)
		{
			for (int g = 0; g < 4; g++)
			{
				for (i = g*8; i < (g+1)*8; i++)
				{
					for(j = f*8; j < (f+1)*8; j++)
					{
						if (i == j)
						{ 
							v1 = A[i][j]; 
							d = i;  
						}
						else
						{	
							v2 = A[i][j]; 
							B[j][i] = v2; 
						}
					}
					if (f == g)
					{ B[d][d] = v1; }
				}
			}
		}
	}
	if (M == 32 && N == 64)
	{
		for (int f = 0; f < 8; f++)
		{
			for (int g = 0; g < 8; g++)
			{
				for(i = f*8; i < (f+1)*8; i++)
				{
					for (j = g*4; j < (g+1)*4; j++)
					{
						v1 = A[i][j]; 
						B[j][i] = v1; 
					}
				}
			}
		}
	}
	if (M == 64 && N == 64)
	{
		int v3, v4, v5, v6, v7, v8;
		for (int f = 0; f < 8; f++)
        {
            for (int g = 0; g < 8; g++)
            {
				for (i = f*8; i < f*8 + 4; i++)
				{
					v1 = A[i][g*8]; 
					v2 = A[i][g*8+1]; 
					v3 = A[i][g*8+2]; 
					v4 = A[i][g*8+3];
					v5 = A[i][g*8+4];
                    v6 = A[i][g*8+5];
                    v7 = A[i][g*8+6];
                    v8 = A[i][g*8+7]; 
					B[g*8][i] = v1; 
					B[g*8+1][i] = v2; 
					B[g*8+2][i] = v3; 
					B[g*8+3][i] = v4; 
					B[g*8][i+4] = v5; 
					B[g*8+1][i+4] = v6; 
					B[g*8 +2][i+4] = v7; 
					B[g*8+3][i+4] = v8;  
				}
				for (i = g*8; i < g*8+4; i++)
				{
					v1 = B[i][f*8+4]; 
					v2 = B[i][f*8+5]; 
					v3 = B[i][f*8+6]; 
					v4 = B[i][f*8+7]; 
					v5 = A[f*8+4][i]; 
					v6 = A[f*8+5][i]; 
					v7 = A[f*8+6][i]; 
					v8 = A[f*8+7][i]; 
					B[i][f*8 + 4] = v5; 
					B[i][f*8+5] = v6; 
					B[i][f*8 + 6] = v7; 
					B[i][f*8+7] = v8;
					B[i+4][f*8] = v1;
                    B[i+4][f*8+1] = v2;
                    B[i+4][f*8 + 2] = v3;
                    B[i+4][f*8+3] = v4;	 	
				}
				for (i = g*8+4; i < g*8+8; i++)
				{
					for (d = 0; d <4; d++)
					{
						B[i][f*8+4+d] = A[f*8+4+d][i]; 
					}
				}

            }
        }
	} 
	if (M== 61 && N == 67)
	{
		for (int f = 0; f < 4; f++)
		{
			for (int g= 0; g < 5; g++)
			{
				for (i = g*16; i < (g+1)*16 && i < 67; i++)
				{
					for (j = f*16; j < (f+1)*16 && j < 61; j++)	
					{
						if (i == j)
						{
							v1 = A[i][j]; 
							d = i; 
						}
						else{
							v2 = A[i][j]; 
							B[j][i] = v2; 
						}
					}
					if (f == g)
					{ B[d][d] = v1; } 
				}
			}
		}
	}
}

char trans1_desc[] = "transpose 1"; 
void trans1(int M, int N, int A[N][M], int B[M][N])
{
	int i, j, n; 
	int v1; 
	if (M == 32 && N == 32)
	{
		//Do all diagonals first 
		for (i = 0; i < N; i++)
		{ B[i][i] = A[i][i];}
		//Do the 8x8 blocks that the diagonal runs through 
		for (n = 0; n < 4; n++)
		{
			for (i = 0; i < 8; i++)
			{
				for (j = 0; j <8; j++)
				{
					B[j + n*8][i + n*8] = A[i+n*8][j + n*8]; 
				}
			}
		}	
		//Do rest of blocks - go block by block  
		for (n = 1; n < 4; n++)
		{
			for (i = 0; i < 8; i++)
			{
				for (j = 0; j < 8; j++)
				{
					v1 = A[i][j + n*8];
					B[j + n*8][i] = v1;  
				}
			}
			for (i = 0; i < 8; i++)
			{
				for(j=0; j < 8; j++)
				{
					B[i][j + n*8] = A[j+n*8][i]; 
				}
			}
		}
		for (n = 2; n < 4; n++)
        {
            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    v1 = A[i + 8][j+ n*8];
                    B[j+ n*8][i+8] = v1;
                }
            }
			for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    v1 = A[j + n*8][i+ 8];
                    B[i+ 8][j+n*8] = v1;
                }
            }
 
        }
		//Last set of blocks 
		for (i = 0; i < 8; i++)
		{
			for (j = 0; j < 8; j++)
			{
				v1 = A[i + 2*8][j+ 3*8];
                B[j+ 3*8][i+2*8] = v1;

			}
		}
		for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    v1 = A[j + 3*8][i+ 2*8];
                    B[i+ 2*8][j+3*8] = v1;
                }
            }

	}
	
}

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
//    registerTransFunction(trans, trans_desc);
	
	//registerTransFunction(trans1, trans1_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

