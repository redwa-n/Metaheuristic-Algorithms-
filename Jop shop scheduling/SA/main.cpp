#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>

using namespace std;

// Parameters for Simulated Annealing
const double INITIAL_TEMPERATURE = 10000;
const double COOLING_RATE = 0.995;
const int MAX_ITERATIONS = 1000;

// Structure for a Task (task on a specific machine with a specific duration)
struct Task {
    int jobID;
    int machineID;
    int duration;
};

// Structure to represent a solution (schedule)
struct Solution {
    vector<int> schedule; // Job sequence
    int makespan;
};

// Random number generator (using std::mt19937)
random_device rd;
mt19937 rng(rd());

// Example Job-Shop Scheduling problem data
vector<vector<Task> > jobs;  // Space added between consecutive right angle brackets

// Number of jobs and machines
int numJobs;
int numMachines = 3;  // Assume 3 machines

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
    shuffle(solution.schedule.begin(), solution.schedule.end(), rng);
    
    solution.makespan = calculateMakespan(solution.schedule);
    return solution;
}

// Get a neighboring solution by swapping two jobs in the schedule
Solution getNeighbor(const Solution& currentSolution) {
    Solution neighbor = currentSolution;
    int pos1 = rng() % neighbor.schedule.size();
    int pos2 = rng() % neighbor.schedule.size();
    swap(neighbor.schedule[pos1], neighbor.schedule[pos2]);
    neighbor.makespan = calculateMakespan(neighbor.schedule);
    return neighbor;
}

// Calculate the acceptance probability
double acceptanceProbability(int oldCost, int newCost, double temperature) {
    if (newCost < oldCost) {
        return 1.0;
    }
    return exp((oldCost - newCost) / temperature);
}

// Main Simulated Annealing function
Solution simulatedAnnealing() {
    Solution currentSolution = generateInitialSolution();
    Solution bestSolution = currentSolution;

    double temperature = INITIAL_TEMPERATURE;

    // Simulated Annealing loop
    for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration) {
        Solution neighbor = getNeighbor(currentSolution);

        if (acceptanceProbability(currentSolution.makespan, neighbor.makespan, temperature) > ((double) rng() / rng.max())) {
            currentSolution = neighbor;
        }

        if (currentSolution.makespan < bestSolution.makespan) {
            bestSolution = currentSolution;
        }

        temperature *= COOLING_RATE;
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

    // Run Simulated Annealing
    clock_t start = clock();
    Solution bestSolution = simulatedAnnealing();
    clock_t end = clock();

    double duration = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    cout << "Best makespan: " << bestSolution.makespan << endl;
    cout << "Execution time: " << duration << " ms" << endl;

    return 0;
}

