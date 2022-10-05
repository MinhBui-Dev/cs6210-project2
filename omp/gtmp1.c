#include <omp.h>
#include "gtmp.h"
#include <stdio.h>

int P;
int count;
int sense;
int local_sense;

void gtmp_init(int num_threads){
    P = num_threads;
    count = P;
    sense = 1;
    local_sense = 1;
}

void gtmp_barrier(){
    # pragma omp parallel num_threads(P) firstprivate(local_sense) shared(count, sense)
    local_sense = !local_sense;
    if (__sync_fetch_and_sub(&count, 1) == 1) {
        count = P;
	sense = local_sense;
    } else {
        while (sense != local_sense);
    }
}

void gtmp_finalize(){

}

