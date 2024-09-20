#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

// Parameters for Ant Colony Optimization
const int NUM_ANTS = 30;
const int MAX_ITERATIONS = 1000;
const double ALPHA = 1.0;
const double BETA = 2.0;
const double EVAPORATION = 0.5;
const double Q = 100.0;

// Structure for a Task
struct Task {
    int jobID;
    int machineID;
    int duration;
};

// Structure to represent a solution
struct Solution {
    vector<int> schedule;
    int makespan;
};

// Random number generator
unsigned int rng_seed = static_cast<unsigned int>(time(0));
int randomInt(int max) {
    rng_seed = rng_seed * 1103515245 + 12345;
    return (rng_seed / 65536) % max;
}

// Example Job-Shop Scheduling problem data
vector< vector<Task> > jobs; // Pre-C++11 style for nested vectors

// Number of jobs, machines, and tasks
int numJobs;
int numMachines = 3;
int numTasks;

// Pheromone matrix
vector< vector<double> > pheromone; // Pre-C++11 style for nested vectors

// Calculate the makespan of a schedule
int calculateMakespan(const vector<int>& schedule) {
    vector<int> machineTime(numMachines, 0);
    vector<int> jobTime(numJobs, 0);

    for (size_t i = 0; i < schedule.size(); ++i) { // No range-based for loop
        int jobID = schedule[i];
        for (size_t j = 0; j < jobs[jobID].size(); ++j) { // Replace range-based loop with standard loop
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

// Custom shuffle function
void customShuffle(vector<int>& vec) {
    for (int i = vec.size() - 1; i > 0; --i) {
        int j = randomInt(i + 1);
        swap(vec[i], vec[j]);
    }
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
    customShuffle(solution.schedule);
    solution.makespan = calculateMakespan(solution.schedule);
    return solution;
}

// Generate a new solution for an ant using probabilistic selection
Solution generateAntSolution() {
    Solution solution;
    vector<bool> visited(numJobs, false);
    solution.schedule.clear();

    for (int i = 0; i < numTasks; ++i) {
        double sumProbabilities = 0.0;
        vector<double> probabilities(numJobs, 0.0);

        for (int j = 0; j < numJobs; ++j) {
            if (!visited[j]) {
                double pheromoneLevel = pow(pheromone[solution.schedule.size()][j], ALPHA);
                // Fixing the issue with `{j}`. Use a proper vector with one element.
                vector<int> singleJob(1, j);
                double heuristicValue = pow(1.0 / (calculateMakespan(singleJob) + 1), BETA);
                probabilities[j] = pheromoneLevel * heuristicValue;
                sumProbabilities += probabilities[j];
            }
        }

        double r = static_cast<double>(randomInt(10000)) / 10000.0 * sumProbabilities;
        double cumulative = 0.0;
        int nextJob = -1;
        for (int j = 0; j < numJobs; ++j) {
            cumulative += probabilities[j];
            if (r <= cumulative) {
                nextJob = j;
                break;
            }
        }

        visited[nextJob] = true;
        solution.schedule.push_back(nextJob);
    }

    solution.makespan = calculateMakespan(solution.schedule);
    return solution;
}

// Update pheromones
void updatePheromone(const vector<Solution>& solutions) {
    for (int i = 0; i < numTasks; ++i) {
        for (int j = 0; j < numTasks; ++j) {
            pheromone[i][j] *= (1 - EVAPORATION);
        }
    }

    for (size_t i = 0; i < solutions.size(); ++i) {
        const Solution& solution = solutions[i];
        for (size_t j = 0; j < solution.schedule.size() - 1; ++j) {
            int jobA = solution.schedule[j];
            int jobB = solution.schedule[j + 1];
            pheromone[jobA][jobB] += Q / solution.makespan;
        }
    }
}

// Main Ant Colony Optimization function
Solution antColonyOptimization() {
    Solution bestSolution = generateInitialSolution();

    for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration) {
        vector<Solution> antSolutions;

        for (int ant = 0; ant < NUM_ANTS; ++ant) {
            Solution antSolution = generateAntSolution();
            antSolutions.push_back(antSolution);

            if (antSolution.makespan < bestSolution.makespan) {
                bestSolution = antSolution;
            }
        }

        updatePheromone(antSolutions);
    }

    return bestSolution;
}

int main() {
    // Initialize jobs data manually without C++11 initializer lists
    jobs.resize(3);
    // Initialize jobs data manually without C++11 initializer lists
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

    // Initialize the pheromone matrix
    pheromone.resize(numTasks);
    for (int i = 0; i < numTasks; ++i) {
        pheromone[i].resize(numTasks, 1.0);
    }

    // Run Ant Colony Optimization
    clock_t start = clock();
    Solution bestSolution = antColonyOptimization();
    clock_t end = clock();

    double duration = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    cout << "Best makespan: " << bestSolution.makespan << endl;
    cout << "Execution time: " << duration << " ms" << endl;

    return 0;
}
