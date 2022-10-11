#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include "gtmpi.h"
#include <omp.h>
#include "gtmp.h"


int main(int argc, char **argv)
{
    int num_processes, num_threads, num_rounds;
    struct timeval start, end, duration;

    MPI_Init(&argc, &argv);

    if (argc < 4)
    {
        fprintf(stderr, "Usage: ./harness [NUM_PROCS] [NUM_THREADS] [NUM_ROUNDS]\n");
        exit(EXIT_FAILURE);
    }

    num_processes = strtol(argv[1], NULL, 10);
    num_threads = strtol(argv[2], NULL, 10);
    num_rounds = strtol(argv[3], NULL, 10);



    gtmpi_init(num_processes);

    int pid;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    if (pid == 0)
    {
        printf("num_processes: %d, num_threads: %d, num_rounds: %d\n", num_processes, num_threads, num_rounds);
    }


    omp_set_dynamic(0);
    if (omp_get_dynamic())
        printf("Warning: dynamic adjustment of threads has been set\n");

    omp_set_num_threads(num_threads);

    gtmp_init(num_threads);


    gettimeofday(&start, NULL);
    for (int k = 0; k < num_rounds; k++)
    {

        printf("Round %d with P-%d\n", k, pid);


        #pragma omp parallel shared(num_threads)
        {
            int tid = omp_get_thread_num();
            printf(" |-- T-%d reached B-%d\n", tid, k);
            gtmp_barrier();
        }

        printf(" |-- P-%d reached B-%d\n", pid, k);
        gtmpi_barrier();
    }
    gettimeofday(&end, NULL);
    timersub(&end, &start, &duration);
    printf("Average time taken: %06ld\n", (duration.tv_sec * 1000000 + duration.tv_usec)/num_rounds);

    gtmp_finalize();
    gtmpi_finalize();
    MPI_Finalize();

    

    return 0;
}