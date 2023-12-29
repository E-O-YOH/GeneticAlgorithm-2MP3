#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "functions.h"

// function returns a double value between min and max
double generate_random(double min, double max) 
{
    double random = (rand()/(double)RAND_MAX)*(max - min) + min;
    return random;
}

// function returns a random integer value
unsigned int generate_int() 
{
    return rand();
}

// function to initialize a randomly distributed population
void generate_population(int POPULATION_SIZE, int NUM_VARIABLES, double population[POPULATION_SIZE][NUM_VARIABLES], double Lbound[NUM_VARIABLES], double Ubound[NUM_VARIABLES]) 
{
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        for (int j = 0; j < NUM_VARIABLES; j++)
        {
            population[i][j] = generate_random(Lbound[j], Ubound[j]);
        }
    }
}

// function to compute the objective function for each member of the population
void compute_objective_function(int POPULATION_SIZE, int NUM_VARIABLES, double population[POPULATION_SIZE][NUM_VARIABLES], double fitness[POPULATION_SIZE])
{
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        fitness[i] = Objective_function(NUM_VARIABLES, population[i]);
    }
}

// function to determine which individuals crossover and which genetic code will make it to the next generation
void crossover(int POPULATION_SIZE, int NUM_VARIABLES, double fitness[POPULATION_SIZE], double new_population[POPULATION_SIZE][NUM_VARIABLES], double population[POPULATION_SIZE][NUM_VARIABLES], double crossover_rate)
{
    int i,j;
    // cycles through each pair of individuals 
    for (i = 0; i < POPULATION_SIZE; i += 2)
    {
        // based on the chance that the parents will have children
        if (generate_random(0,1) < crossover_rate)
        {
            double parent1[NUM_VARIABLES];
            double parent2[NUM_VARIABLES];

            for (j = 0; j < NUM_VARIABLES; j++)
            {
                parent1[j] = population[i][j];
                parent2[j] = population[i+1][j];
            }

            // generates a random number between 1 and (NUM_VARIABLES - 1) to determine which index the crossover will begin
            int crosspoint = generate_int() % (NUM_VARIABLES - 1) + 1; 

            double child1[NUM_VARIABLES];
            double child2[NUM_VARIABLES];

            // keep the part before crossover
            for (j = 0; j < crosspoint; j++)
            {
                child1[j] = parent1[j];
                child2[j] = parent2[j];
            }

            // switch the part after crossover
            for (j = crosspoint; j < NUM_VARIABLES; j++)
            {
                child1[j] = parent2[j];
                child2[j] = parent1[j];
            }

            for (j=0; j < NUM_VARIABLES; j++)
            {
                new_population[i][j] = child1[j];
                new_population[i+1][j] = child2[j];
            }
        }
        // if they don't have children move their genetic code straight to the next generation
        else
        {
            for (j=0; j < NUM_VARIABLES; j++)
            {
                new_population[i][j] = population[i][j];
                new_population[i+1][j] = population[i][j];
            }
        }
    }
}

// function to determine which members of the population will mutate
void mutate(int POPULATION_SIZE, int NUM_VARIABLES, double new_population[POPULATION_SIZE][NUM_VARIABLES], double population[POPULATION_SIZE][NUM_VARIABLES], double Lbound[NUM_VARIABLES], double Ubound[NUM_VARIABLES], double mutate_rate)
{
    int i,j;
    int mark = 0;

    // variable "mark" is used to ensure mutate_rate % of genes will mutate, avoiding the same gene from mutating multiple times in the same population which would be useless
    for (i = 0; i < NUM_VARIABLES; i++)
    {
        mark += fabs(Lbound[i]) + fabs(Ubound[i]);
    }
    
    int total_gene = POPULATION_SIZE*NUM_VARIABLES;
    int total_gene_mutate = total_gene*mutate_rate;

    // marks mutate_rate % of all genes with the value of variable "mark"
    for (i = 0; i < total_gene_mutate; i++)
    {
        // randomly determine which gene to mutate
        int gene_to_mutate = generate_int() % (total_gene - 1);
        int row = gene_to_mutate / NUM_VARIABLES;
        int col = gene_to_mutate % NUM_VARIABLES;

        if (new_population[row][col] != mark)
        {
            new_population[row][col] = mark;
        }
        else {i--;}
    }

    // go through each individual and if they are marked for mutation, mutate that gene somewhere in the range applicable. If not move then to the new population unchanged
    for (i = 0; i < POPULATION_SIZE; i++)
    {
        for (j = 0; j < NUM_VARIABLES; j++)
        {
            if (new_population[i][j] == mark)
            {
                new_population[i][j] = generate_random(Lbound[j], Ubound[j]);
            }
            else
            {
                new_population[i][j] = population[i][j];
            }
        }
    }
}