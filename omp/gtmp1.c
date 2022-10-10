#include <omp.h>
#include "gtmp.h"
#include <stdio.h>

typedef enum { false, true } bool;

int P;
int count;
bool sense;

void gtmp_init(int num_threads){
    P = num_threads;
    count = P;
    sense = false;
}

void gtmp_barrier(){
    bool local_sense = !sense;
    if (__sync_fetch_and_sub(&count, 1) == 1) {
        count = P;
	    sense = local_sense;
    } else {
        while (local_sense != sense);
    }
}

void gtmp_finalize(){

}

