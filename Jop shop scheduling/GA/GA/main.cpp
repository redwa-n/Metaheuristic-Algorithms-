#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>


using namespace std;

// Parameters for Genetic Algorithm
const int POPULATION_SIZE = 30;
const int MAX_GENERATIONS = 1000;
const double CROSSOVER_RATE = 0.8;
const double MUTATION_RATE = 0.1;

// Structure for a Task
struct Task {
    int jobID;
    int machineID;
    int duration;
};

// Structure to represent a solution (individual)
struct Solution {
    vector<int> schedule;
    int makespan;
};

// Random number generator (using std::mt19937)
random_device rd;
mt19937 rng(rd());

// Example Job-Shop Scheduling problem data
vector<vector<Task> > jobs;  // Space between '>' required for pre-C++11

// Number of jobs, machines, and tasks
int numJobs;
int numMachines = 3;
int numTasks;

// Calculate the makespan of a schedule
int calculateMakespan(const vector<int>& schedule) {
    vector<int> machineTime(numMachines, 0);
    vector<int> jobTime(numJobs, 0);

    for (size_t i = 0; i < schedule.size(); ++i) {
        int jobID = schedule[i];
        for (size_t j = 0; j < jobs[jobID].size(); ++j) {
            const Task& task = jobs[jobID][j];
            int machineID = task.machineID;
            int start = max(machineTime[machineID], jobTime[jobID]);
            int end = start + task.duration;
            machineTime[machineID] = end;
            jobTime[jobID] = end;
        }
    }
    return *max_element(machineTime.begin(), machineTime.end());
}

// Generate a random initial solution
Solution generateInitialSolution() {
    Solution solution;
    solution.schedule.resize(numJobs * numMachines);
    for (int j = 0; j < numJobs; ++j) {
        for (int k = 0; k < numMachines; ++k) {
            solution.schedule[j * numMachines + k] = j;
        }
    }
    
    // Use std::shuffle instead of random_shuffle
    std::shuffle(solution.schedule.begin(), solution.schedule.end(), rng);
    
    solution.makespan = calculateMakespan(solution.schedule);
    return solution;
}

// Tournament selection
Solution tournamentSelection(const vector<Solution>& population) {
    int tournamentSize = 3;
    Solution best = population[rng() % POPULATION_SIZE];
    for (int i = 1; i < tournamentSize; ++i) {
        Solution contender = population[rng() % POPULATION_SIZE];
        if (contender.makespan < best.makespan) {
            best = contender;
        }
    }
    return best;
}

// Crossover two parents to produce two offspring
pair<Solution, Solution> crossover(const Solution& parent1, const Solution& parent2) {
    Solution offspring1 = parent1;
    Solution offspring2 = parent2;

    if ((double)(rng() % 100) / 100.0 < CROSSOVER_RATE) {
        int crossoverPoint = rng() % numTasks;

        for (int i = 0; i < crossoverPoint; ++i) {
            swap(offspring1.schedule[i], offspring2.schedule[i]);
        }
    }

    offspring1.makespan = calculateMakespan(offspring1.schedule);
    offspring2.makespan = calculateMakespan(offspring2.schedule);

    return make_pair(offspring1, offspring2);
}

// Mutate a solution
void mutate(Solution& solution) {
    if ((double)(rng() % 100) / 100.0 < MUTATION_RATE) {
        int index1 = rng() % numTasks;
        int index2 = rng() % numTasks;
        swap(solution.schedule[index1], solution.schedule[index2]);
        solution.makespan = calculateMakespan(solution.schedule);
    }
}

// Main Genetic Algorithm function
Solution geneticAlgorithm() {
    // Step 1: Initialize population
    vector<Solution> population;
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        population.push_back(generateInitialSolution());
    }

    Solution bestSolution = population[0];

    // Step 2: Evolution loop
    for (int generation = 0; generation < MAX_GENERATIONS; ++generation) {
        vector<Solution> newPopulation;

        // Step 3: Selection, crossover, mutation
        while (newPopulation.size() < POPULATION_SIZE) {
            Solution parent1 = tournamentSelection(population);
            Solution parent2 = tournamentSelection(population);

            pair<Solution, Solution> offspring = crossover(parent1, parent2);

            mutate(offspring.first);
            mutate(offspring.second);

            newPopulation.push_back(offspring.first);
            newPopulation.push_back(offspring.second);
        }

        // Step 4: Replace population with new population
        population = newPopulation;

        // Step 5: Update best solution
        for (size_t i = 0; i < population.size(); ++i) {
            const Solution& sol = population[i];
            if (sol.makespan < bestSolution.makespan) {
                bestSolution = sol;
            }
        }
    }

    return bestSolution;
}

int main() {
    // Initialize jobs data manually
    jobs.resize(3);
    Task task1 = {0, 0, 3};
    Task task2 = {0, 1, 2};
    Task task3 = {0, 2, 2};
    jobs[0].push_back(task1);
    jobs[0].push_back(task2);
    jobs[0].push_back(task3);

    Task task4 = {1, 0, 2};
    Task task5 = {1, 1, 1};
    Task task6 = {1, 2, 4};
    jobs[1].push_back(task4);
    jobs[1].push_back(task5);
    jobs[1].push_back(task6);

    Task task7 = {2, 0, 4};
    Task task8 = {2, 1, 3};
    Task task9 = {2, 2, 3};
    jobs[2].push_back(task7);
    jobs[2].push_back(task8);
    jobs[2].push_back(task9);

    numJobs = jobs.size();
    numTasks = numJobs * numMachines;

    // Run Genetic Algorithm
    clock_t start = clock();
    Solution bestSolution = geneticAlgorithm();
    clock_t end = clock();

    double duration = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    cout << "Best makespan: " << bestSolution.makespan << endl;
    cout << "Execution time: " << duration << " ms" << endl;

    return 0;
}
