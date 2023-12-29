#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "functions.h"


int main(int argc, char *argv[]) 
{
    
    // error and input handling

    if (argc != 6) 
    {
        printf("Invalid Input. Please try again with the following format:\n%s <population_size> <max_generations> <crossover_rate> <mutate_rate> <stop_criteria>\n", argv[0]);
        return 1;
    }
    
    int POPULATION_SIZE, MAX_GENERATIONS;
    double crossover_rate, mutate_rate, stop_criteria;
    
    if (sscanf(argv[1], "%d", &POPULATION_SIZE) != 1) 
    {
        printf("Invalid argument for POPULATION_SIZE: %s\nPlease enter an integer value for POPULATION_SIZE.\n", argv[1]);
        return 1; 
    }

    if (sscanf(argv[2], "%d", &MAX_GENERATIONS) != 1) 
    {
        printf("Invalid argument for MAX_GENERATIONS: %s\nPlease enter an integer value for MAX_GENERATIONS.\n", argv[2]);
        return 1; 
    }

    if (sscanf(argv[3], "%lf", &crossover_rate) != 1) 
    {
        printf("Invalid argument for crossover_rate: %s\nPlease enter a float value for crossover_rate.\n", argv[3]);
        return 1; 
    }

    if (sscanf(argv[4], "%lf", &mutate_rate) != 1) 
    {
        printf("Invalid argument for mutate_rate: %s\nPlease enter a float value for mutate_rate.\n", argv[4]);
        return 1; 
    }

    if (sscanf(argv[5], "%lf", &stop_criteria) != 1) 
    {
        printf("Invalid argument for stop_criteria: %s\nPlease enter a float value for stop_criteria.\n", argv[5]);
        return 1; 
    }

    POPULATION_SIZE = atoi(argv[1]);    
    MAX_GENERATIONS = atoi(argv[2]);
    crossover_rate = atof(argv[3]);
    mutate_rate = atof(argv[4]);
    stop_criteria = atof(argv[5]);

    // ###################################################################################
    // hard coded values for this test case
    int NUM_VARIABLES = 2;
    // the lower bounds of variables
    double Lbound[] = {-5.0, -5.0};
    // the upper bounds of variable
    double Ubound[] = {5.0, 5.0};
    // ###################################################################################

    // initial print outs
    printf("\nGenetic Algorithm is initiated.\n");
    printf("------------------------------------------------\n");
    printf("The number of variables: %d\n", NUM_VARIABLES);
    printf("Lower bounds: [");
    for (int i = 0; i < NUM_VARIABLES; i++)
    {
        printf("%.2lf",Lbound[i]);
        if (i != NUM_VARIABLES-1)
        {
            printf(", ");
        }
    }
    printf("]\n");
    printf("Upper bounds: [");
    for (int i = 0; i < NUM_VARIABLES; i++)
    {
        printf("%.2lf",Ubound[i]);
        if (i != NUM_VARIABLES-1)
        {
            printf(", ");
        }
    }
    printf("]\n");
    printf("\nPopulation Size:\t %d\n", POPULATION_SIZE);
    printf("Max Generations:\t %d\n", MAX_GENERATIONS);
    printf("Crossover Rate:\t\t %.3lf\n", crossover_rate);
    printf("Mutation Rate:\t\t %.3lf\n", mutate_rate);
    printf("Stopping Criteria:\t %.2e\n", stop_criteria);

    // seed rand() and start clock to calculate CPU Time
    srand(time(NULL));
    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock();

    // declaring all the arrays needed, crossover_pop is the generation after crossover and before mutation
    double initial_pop[POPULATION_SIZE][NUM_VARIABLES];
    double survived_pop[POPULATION_SIZE][NUM_VARIABLES];
    double crossover_pop[POPULATION_SIZE][NUM_VARIABLES];
    double fitness[POPULATION_SIZE];
    double fitness_probs[POPULATION_SIZE];
    double best_solution[NUM_VARIABLES];

    generate_population(POPULATION_SIZE, NUM_VARIABLES, initial_pop, Lbound, Ubound);

    double fitness_min = 100;
    int generation;
    // counter for how many generations have elapsed since the last 'better' result
    int no_change_count = 0; 

    // where the genetic algorithm begins, will terminate if the stop criteria is met 10 times or the MAX_GENERATIONS is reached
    for (generation = 0; generation < MAX_GENERATIONS; generation++)
    {
        compute_objective_function(POPULATION_SIZE, NUM_VARIABLES, initial_pop, fitness);
        int i,j,k;
        double fitness_old = fitness_min;
        double fitness_probs_total = 0;

        // determines the probability that each individual will survive and keeps track of the best solution
        for (i = 0; i < POPULATION_SIZE; i++)
        {
            if (fitness[i] < fitness_min)
            {
                fitness_min = fitness[i];
                for (j = 0; j < NUM_VARIABLES; j++)
                {
                    best_solution[j] = initial_pop[i][j];
                }
            }
            fitness_probs[i] = 1/(fitness[i] + 1e-16);
            fitness_probs_total += fitness_probs[i];
        }

        // determines if the algorithm should terminate based on the stop criteria and the no change counter
        if (fitness_old - fitness_min < stop_criteria)
        {
            no_change_count++;
            if (no_change_count > 10)
            {
                break;
            }
        }
        else
        {
            no_change_count = 0;
        }

        // finishes determining the probability that each individual will survive
        for (i = 0; i < POPULATION_SIZE; i++)
        {
            fitness_probs[i] /= fitness_probs_total;
            if (i != 0)
            {
                fitness_probs[i] += fitness_probs[i-1];
            }
        }

        // decides which individuals make it from the initial population to the survived population
        for (i = 0; i < POPULATION_SIZE; i++)
        {
            double random = generate_random(0,1);
            for (j = 0; j < POPULATION_SIZE; j++)
            {
                if (fitness_probs[j] > random)
                {
                    for (k = 0; k < NUM_VARIABLES; k++)
                    {
                        survived_pop[i][k] = initial_pop[j][k];
                    }
                    break;
                }
            }
        }

        // crossover function determining which parents genetic code continues from the survived to the crossover population
        crossover(POPULATION_SIZE, NUM_VARIABLES, fitness, crossover_pop, survived_pop, crossover_rate);

        // mutation function determining which individuals will mutate before they all become the initial population again
        mutate(POPULATION_SIZE, NUM_VARIABLES, initial_pop, crossover_pop, Lbound, Ubound, mutate_rate);
        // now having the new population, it goes to the beginning of loop to re-compute all again
    }

    // ###################################################################################
    // here the CPU time taken for the code is printed
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("\nResults\n------------------------------------------------\n");
    printf("CPU Time: %f seconds\n", cpu_time_used);
    // ###################################################################################

    // final print outs including the best solution, fitness, and number of generations taken
    printf("Best Solution Found: (");
    for (int i = 0; i < NUM_VARIABLES; i++)
    {
        printf("%.15lf",best_solution[i]);
        if (i != NUM_VARIABLES-1)
        {
            printf(", ");
        }
    }
    printf(")\n");
    printf("Best Fitness: %.15lf\n", fitness_min);
    printf("Generations: %d\n", generation);
    printf("------------------------------------------------\n");

    return 0;
}
