#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include "gtmpi.h"

int main(int argc, char** argv)
{
  int num_processes, num_rounds = 10;
  struct timeval start, end, duration;

  MPI_Init(&argc, &argv);
  
  if (argc < 2){
    fprintf(stderr, "Usage: ./harness [NUM_PROCS]\n");
    exit(EXIT_FAILURE);
  }

  num_processes = strtol(argv[1], NULL, 10);

  gtmpi_init(num_processes);
  gettimeofday(&start, NULL);
  int k;
  for(k = 0; k < num_rounds; k++){
    gtmpi_barrier();
  }

  gettimeofday(&end, NULL);
  timersub(&end, &start, &duration);
  printf("Average time taken: %06ld\n", (duration.tv_sec * 1000000 + duration.tv_usec)/num_rounds);
  gtmpi_finalize();  

  MPI_Finalize();

  return 0;
}
