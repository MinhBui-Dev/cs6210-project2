#include <omp.h>
#include "gtmp.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define winner 0
#define loser 1
#define bye 2
#define champion 3
#define dropout 4

typedef enum { false, true } bool;

typedef struct record
{
    int role;
    bool *opponentBoolean;
    bool flag;
} record;

record *tournamentRecord;
bool globalSense = true;

void gtmp_init(int num_threads)
{
    int p = num_threads;
    int numRound = ceil(log2(num_threads)) + 1;
    printf("num_threads: %d\nnumRound: %d\n", p, numRound);
    tournamentRecord = (record *)malloc(sizeof(record) * num_threads * numRound);

    // initialize all flags to false
    printf("Initializing flags to false\n");
    for (int i = 0; i < num_threads * numRound; i++)
    {
        tournamentRecord[i].flag = false;
    }

    // initialize role to each node in the tournament
    for (int i = 0; i < num_threads; i++)
    {
        for (int k = 0; k < numRound; k++)
        {
            int firstPower = pow(2, k);
            int secondPower = pow(2, k - 1);
            int index = i * numRound + k; // round[i][k] = i * numRound + k

            if (k > 0 && (i % firstPower) == 0 && i + secondPower < p && firstPower < p)
            {
                tournamentRecord[index].role = winner;
                // printf("Node %d is a winner in round %d\n", i, k);
            }
            if (k > 0 && (i % firstPower) == 0 && i + secondPower >= p)
            {
                tournamentRecord[index].role = bye;
                // printf("Node %d is a bye in round %d\n", i, k);
            }
            if (k > 0 && (i % firstPower) == secondPower)
            {
                tournamentRecord[index].role = loser;
                // printf("Node %d is a loser in round %d\n", i, k);
            }
            if (k > 0 && i == 0 && firstPower >= p)
            {
                tournamentRecord[index].role = champion;
                // printf("Node %d is a champion in round %d\n", i, k);
            }
            if (k == 0)
            {
                tournamentRecord[index].role = dropout;
                // printf("Node %d is a dropout in round %d\n", i, k);
            }

            if (tournamentRecord[index].role == loser)
            {
                tournamentRecord[index].opponentBoolean = &tournamentRecord[(i - secondPower) * numRound + k].flag;
            }
            else if (tournamentRecord[index].role == winner || tournamentRecord[index].role == champion)
            {
                tournamentRecord[index].opponentBoolean = &tournamentRecord[(i + secondPower) * numRound + k].flag;
            }
        }
    }
    /* printf("Initialization complete\n\n");

    for (int i = 0; i < num_threads * numRound; i++)
    {
        int round = i % numRound;
        int node = (i - round) / numRound;
        printf("At Index[%d]: Node %d in round %d has role %d\n", i, node, round, tournamentRecord[i].role);
    } */
}

void gtmp_barrier()
{
    int num_threads = omp_get_num_threads();
    int vpid = omp_get_thread_num();
    int round = 1;
    int numRound = ceil(log2(num_threads)) + 1;

    bool sense = globalSense;

    // arrival loop
    while (1)
    {
        if (tournamentRecord[vpid * numRound + round].role == loser)
        {
            *(tournamentRecord[vpid * numRound + round].opponentBoolean) = sense;
            // printf("Node %d set winner flag and is waiting\n", vpid);
            while (tournamentRecord[vpid * numRound + round].flag != sense)
            {
                // spin
            }
            break;
        }

        if (tournamentRecord[vpid * numRound + round].role == winner)
        {
            // printf("Node %d is winner waiting for loser\n", vpid);
            while (tournamentRecord[vpid * numRound + round].flag != sense)
            {
                // spin
            }
        }

        if (tournamentRecord[vpid * numRound + round].role == champion)
        {
            // printf("Node %d is champion waiting for loser\n", vpid);
            while (tournamentRecord[vpid * numRound + round].flag != sense)
            {
                // spin
            }
            // printf("Node %d is champion setting loser's flag\n", vpid);
            *tournamentRecord[vpid * numRound + round].opponentBoolean = sense;
            break;
        }

        round++;
    }

    // wake up loop
    while (1)
    {
        round--;
        // notify loser
        if (tournamentRecord[vpid * numRound + round].role == winner)
        {
            // printf("Node %d is winner setting loser's flag\n", vpid);
            *tournamentRecord[vpid * numRound + round].opponentBoolean = sense;
        }

        // everyone is at round 0
        if (tournamentRecord[vpid * numRound + round].role == dropout)
        {
            break;
        }
    }
    globalSense = !sense;
}

void gtmp_finalize()
{
    free(tournamentRecord);
}
