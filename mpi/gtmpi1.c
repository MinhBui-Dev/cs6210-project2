#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include "gtmpi.h"

int P;
int processor_index;
MPI_Request mpi_request;
MPI_Status mpi_status;

void gtmpi_init(int num_processes){
    P = num_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &processor_index);
}

void gtmpi_barrier(){
   int sense = 1;
    char msg = 'a';
    printf("%i\n", P);
    printf("%i\n", (int)log2(P));
    for (int i = 0; i < log2(P); i++) {
        printf("made it 2\n");
	int send_node_index = (processor_index + (int) pow(2, i)) % P;
	printf("processor %i sending to processor %i\n", processor_index, send_node_index);
	int recv_node_index = (processor_index - (int) pow(2, i)) % P;
	printf("processor %i sending to processor %i\n", processor_index, recv_node_index);
	MPI_Isend(&msg, 1, MPI_CHAR, send_node_index, 1, MPI_COMM_WORLD, &mpi_request);
	MPI_Recv(&msg, 1, MPI_CHAR, recv_node_index, 1, MPI_COMM_WORLD, &mpi_status);
    }
    sense = !sense;
    printf("processor %i passed barrier\n", processor_index);
}

void gtmpi_finalize(){

}
