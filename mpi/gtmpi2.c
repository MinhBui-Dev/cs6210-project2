#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>
#include "gtmpi.h"
#include <math.h>

#define winner 0
#define loser 1
#define bye 2
#define champion 3
#define dropout 4

typedef struct record
{
    int role;
    int opponentIndex;
} record;

record *tournamentRecord;

void gtmpi_init(int num_processes)
{
    int p = num_processes;
    int numRound = ceil(log2(p)) + 1;
    // printf("Num_process: %d\nnumRound: %d\n", p, numRound);
    tournamentRecord = (record *)malloc(sizeof(record) * p * numRound);

    // initialize role to each node in the tournament
    for (int i = 0; i < p; i++)
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

            // initialize opponent to process index
            if (tournamentRecord[index].role == loser)
            {
                tournamentRecord[index].opponentIndex = (i - secondPower);
                // printf("Node %d is a loser in round %d and opponent is %d\n", i, k, tournamentRecord[index].opponentIndex);
            }
            else if (tournamentRecord[index].role == winner || tournamentRecord[index].role == champion)
            {
                tournamentRecord[index].opponentIndex = (i + secondPower);
                /* if (tournamentRecord[index].role == champion)
                {
                    printf("Node %d is a champion in round %d and opponent is %d\n", i, k, tournamentRecord[index].opponentIndex);
                }
                else
                {
                    printf("Node %d is a winner in round %d and opponent is %d\n", i, k, tournamentRecord[index].opponentIndex);
                }     */
            }
        }
    }
}

void gtmpi_barrier()
{
    int id, p;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    // printf("Num_process: %d in barrier\n", p);
    int numRound = ceil(log2(p)) + 1;
    int round = 1;

    // arrival loop
    while (1)
    {
        if (tournamentRecord[id * numRound + round].role == loser)
        {
            // send message to opponent
            // printf("%d is a loser in round %d and opponent is %d\n", id, round, tournamentRecord[id * numRound + round].opponentIndex);
            MPI_Send(NULL, 0, MPI_INT, tournamentRecord[id * numRound + round].opponentIndex, 0, MPI_COMM_WORLD);
            // printf("Node %d set winner flag and is waiting\n", pid);
            // printf("%d is waiting in round %d\n", id, round);
            // wait for winner to send message
            MPI_Recv(NULL, 0, MPI_INT, tournamentRecord[id * numRound + round].opponentIndex, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            break;
        }

        if (tournamentRecord[id * numRound + round].role == winner)
        {
            // printf("Node %d is winner waiting for loser\n", pid);
            // wait for loser to send message
            // printf("%d is a winner in round %d and opponent is %d\n", id, round, tournamentRecord[id * numRound + round].opponentIndex);
            MPI_Recv(NULL, 0, MPI_INT, tournamentRecord[id * numRound + round].opponentIndex, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        if (tournamentRecord[id * numRound + round].role == champion)
        {
            // printf("Node %d is champion waiting for loser\n", pid);
            // wait for loser to send message
            // printf("%d is a champion in round %d and waiting for opponent: %d\n", id, round, tournamentRecord[id * numRound + round].opponentIndex);
            MPI_Recv(NULL, 0, MPI_INT, tournamentRecord[id * numRound + round].opponentIndex, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("Node %d is champion setting loser's flag\n", pid);
            // set loser's flag
            // printf("%d is a champion in round %d and is sending to opponent: %d\n", id, round, tournamentRecord[id * numRound + round].opponentIndex);
            MPI_Send(NULL, 0, MPI_INT, tournamentRecord[id * numRound + round].opponentIndex, 0, MPI_COMM_WORLD);
            break;
        }

        round++;
    }

    // wake up loop
    while (1)
    {
        round--;
        // notify loser
        if (tournamentRecord[id * numRound + round].role == winner)
        {
            // printf("Node %d is winner setting loser's flag\n", pid);
            // set message to loser
            // printf("%d is a winner in round %d and is sending to opponent: %d\n", id, round, tournamentRecord[id * numRound + round].opponentIndex);
            MPI_Send(NULL, 0, MPI_INT, tournamentRecord[id * numRound + round].opponentIndex, 0, MPI_COMM_WORLD);
        }

        // everyone is at round 0
        if (tournamentRecord[id * numRound + round].role == dropout)
        {
            break;
        }
    }
}

void gtmpi_finalize()
{
    free(tournamentRecord);
}
