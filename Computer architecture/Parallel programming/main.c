#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Processor	Intel(R) Core(TM) i5-10210U CPU @ 1.60GHz, 2112 Mhz, 4 Core(s), 8 Logical Processor(s)

#define s1 256000 / sizeof(char)
#define s2 1000000 / sizeof(char)
#define s3 6000000 / sizeof(char)
#define N_ITER 10000

double fun(int size, int bytes);

int main()
{
    printf("A:\n");
    double time_A = fun(s1, 1);

    printf("\n\n\nB:\n");
    double time_B = fun(2 * s1, 64);

    printf("\n\n\nC:\n");
    double time_C = fun(2 * s2, 64);

    printf("\n\n\nD:\n");
    double time_D = fun(2 * s3, 64);

    printf("\nB/A = %f\n", time_B/time_A);
    printf("C/B = %f\n", time_C/time_B);
    printf("D/C = %f\n", time_D/time_C);

    return 0;
}

double fun(int size, int bytes)
{
    int sum = 0;
    clock_t start, end;
    double cpu_time;
    char* spremnik = (char*)malloc(size*sizeof(char));
    if(spremnik == NULL)
        printf("ERROR!!!!\n");
    for(int i = 0; i < size; i++)
        spremnik[i] = 0;

    start = clock();
    for(int j = 0; j < N_ITER; j++)
    {
        for(int i = 0; i < size; i += bytes)
            spremnik[i] += 1;
    }
    end = clock();
    cpu_time = ((double) (end - start)) / CLOCKS_PER_SEC;

    for(int i = 0; i < size; i++)
        sum += spremnik[i];

    printf("Sum: %d\nCpu time used: %f seconds\n", sum, cpu_time);
    free(spremnik);

    printf("Bandwidth: %f MB/s\n\n\n", (size/bytes) / cpu_time);

    return cpu_time;
}