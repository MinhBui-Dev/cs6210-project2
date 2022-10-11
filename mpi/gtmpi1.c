#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include "gtmpi.h"

typedef enum { false, true } bool;

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
    bool sense = false;
    char msg = 'a';
    for (int i = 0; i < log2(P); i++) {
	// int send_node_index = modulo_euclidian(processor_index + (int) pow(2, i), P);
	// int recv_node_index = modulo_euclidian(processor_index - (int) pow(2, i), P);
	int send_node_index = (processor_index - (int) pow(2, i) + P) % P;
	int recv_node_index = (processor_index + (int) pow(2, i) + P) % P;
	MPI_Isend(&msg, 1, MPI_CHAR, send_node_index, 1, MPI_COMM_WORLD, &mpi_request);
	MPI_Recv(&msg, 1, MPI_CHAR, recv_node_index, 1, MPI_COMM_WORLD, &mpi_status);
    }
    sense = !sense;
}

void gtmpi_finalize(){

}
