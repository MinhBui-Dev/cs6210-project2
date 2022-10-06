#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>
#include "gtmpi.h"

int P;
int processor_index;

void gtmpi_init(int num_processes){
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &processor_index);
}

void gtmpi_barrier(){
    //  type flags = record
    //      myflags : array [0..1] of array [0..LogP-1] of Boolean
    //      partnerflags : array [0..1] of array [0..LogP-1] of ^Boolean
    //
    //  processor private parity : integer := 0
    //  processor private sense : Boolean := true
    //  processor private localflags : ^flags
    //  shared allnodes : array [0..P-1] of flags
    //  // allnodes[i] is allocated in shared memory
    //  // locally accessible to processor i
    //  
    //  // on processor i, localflags points to allnodes[i]
    //  // initially allnodes[i].myflags[r][k] is false for all i, r, k
    //  // if j = (i+2^k) mod P, then for r = 0, 1:
    //  //     allnodes[i].partnerflags[r][k] points to allnodes[j].myflags[r][k]
    //
    //  procedure dissemination_barrier
    //      for instance : integer := 0 to LogP-1
    //          localflags^.partnerflags[parity][instance]^ := sense
    //          repeat until localflags^.myflags[parity][instance] = sense
    //      if parity = 1
    //          sense := not sense
    //      parity := 1 - parity
}

void gtmpi_finalize(){

}
