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

/* 
 * Code snippet for calculating euclidian modulo from https://stackoverflow.com/a/57544473
 */
int modulo_euclidian(int a, int b) {
    int m = a % b;
    if (m < 0) {
        m = (b < 0) ? m - b : m + b;
    }
    return m;
}

void gtmpi_barrier(){
    bool sense = false;
    char msg = 'a';
    for (int i = 0; i < log2(P); i++) {
	int send_node_index = modulo_euclidian(processor_index + (int) pow(2, i), P);
	int recv_node_index = modulo_euclidian(processor_index - (int) pow(2, i), P);
	MPI_Isend(&msg, 1, MPI_CHAR, send_node_index, 1, MPI_COMM_WORLD, &mpi_request);
	MPI_Recv(&msg, 1, MPI_CHAR, recv_node_index, 1, MPI_COMM_WORLD, &mpi_status);
    }
    sense = !sense;
}

void gtmpi_finalize(){

}
