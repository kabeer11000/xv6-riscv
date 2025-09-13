#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 2

static unsigned int seed = 1;

// Utility functions for mat print mat gen
int random()
{
    seed = seed * 529869860 + 735;
    return (seed / 636907) % 328;
}

void seedRandom(int s)
{
    seed = s;
}

void genMat(int matrix[N][N], int n)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = random() % 10;
}

void printMat(int matrix[N][N], int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            printf("%d ", matrix[i][j]);
        printf("\n");
    }
}

struct R
{ 
    // computed dot product
    int value;
    // row this result belongs to
    int row;
    // column this result belongs to
    int col;
};

// Function to multiply one row with one col
// A[row][0..N-1] * B[0..N-1][col] = single result value
int mmult(int A[N][N], int B[N][N], int row, int col)
{
    int result = 0;
    printf("Child mmult row %d * col %d: ", row, col);
    for (int k = 0; k < N; k++)
    {
        printf("A[%d][%d]*B[%d][%d] + ", row, k, k, col);
        result += A[row][k] * B[k][col];
    }
    printf(" = %d\n", result);
    return result;
}

int main(void)
{
    seedRandom(120);

    int A[N][N];
    int B[N][N];
    int S[N][N];

    // Initialize result matrix to 0
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            S[i][j] = 0;

    printf("Matrix A:\n");
    genMat(A, N);
    printMat(A, N);

    printf("\nMatrix B:\n");
    genMat(B, N);
    printMat(B, N);

    printf("\nEach child computes one dot product:\n");

    // since each child is calculating one output element we need one pipe for every child.
    int pipes[N * N][2];
    int child_count = 0;

    for (int rowIndex = 0; rowIndex < N; rowIndex++)
    {
        for (int colIndex = 0; colIndex < N; colIndex++)
        {

            // Create pipe for this child
            if (pipe(pipes[child_count]) < 0)
            {
                printf("Pipe creation failed.\n");
                exit(1);
            }

            printf("Child %d for S[%d][%d]\n", child_count, rowIndex, colIndex);

            int pid = fork();
            if (pid < 0)
            {
                printf("Failed to fork.\n");
                exit(1);
            }
            else if (pid == 0)
            {
                // CHILD PROCESS
                close(pipes[child_count][0]);

                int result = mmult(A, B, rowIndex, colIndex);

                struct R res;
                res.value = result;
                res.row = rowIndex;
                res.col = colIndex;

                // Send result back
                write(pipes[child_count][1], &res, sizeof(struct R));
                close(pipes[child_count][1]);

                printf("Child %d finished: S[%d][%d] = %d\n",
                       getpid(), rowIndex, colIndex, result);
                exit(0);
            }
            else
            {
                // nesesary wait call
                wait(&pid);
                // PARENT PROCESS
                close(pipes[child_count][1]); // Close write-end
                child_count++;
            }
        }
    }

    printf("\nCreated %d child processes. appending results.\n", child_count);
    for (int i = 0; i < child_count; i++)
    {
        struct R res;

        read(pipes[i][0], &res, sizeof(struct R));
        close(pipes[i][0]);
        S[res.row][res.col] = res.value;

        printf("Appended: S[%d][%d] = %d\n", res.row, res.col, res.value);
    }

    printf("\nFinal Result Matrix S = A * B:\n");
    printMat(S, N);
    exit(0);
}