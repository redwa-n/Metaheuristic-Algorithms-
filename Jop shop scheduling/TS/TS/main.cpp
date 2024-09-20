#include <iostream>
#include <vector>
#include <algorithm>  // For std::shuffle
#include <queue>
#include <ctime>      // For srand() and time()
#include <random>     // For default_random_engine

using namespace std;

// Constants for Tabu Search
const int TABU_TENURE = 10;
const int MAX_ITERATIONS = 1000;

// Structure for a Task (task on a specific machine with a specific duration)
struct Task {
    int jobID;
    int machineID;
    int duration;
};

// Structure to represent a solution (schedule)
struct Solution {
    vector<int> schedule;  // Job sequence
    int makespan;
};

// Random number generation with pre-C++11 method (rand)
void initializeRandom() {
    srand(static_cast<unsigned int>(time(0)));
}

// Example Job-Shop Scheduling problem data
vector<vector<Task> > jobs;  // Added space between right angle brackets

// Number of jobs and machines
int numJobs;
const int NUM_MACHINES = 3;  // Assume 3 machines

// Calculate the makespan of a schedule
int calculateMakespan(const vector<int>& schedule) {
    vector<int> machineTime(NUM_MACHINES, 0);
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
    solution.schedule.resize(numJobs * NUM_MACHINES);
    for (int j = 0; j < numJobs; ++j) {
        for (int k = 0; k < NUM_MACHINES; ++k) {
            solution.schedule[j * NUM_MACHINES + k] = j;
        }
    }

    // Use std::shuffle with std::default_random_engine
    std::default_random_engine rng(static_cast<unsigned>(time(0)));
    shuffle(solution.schedule.begin(), solution.schedule.end(), rng);

    solution.makespan = calculateMakespan(solution.schedule);
    return solution;
}

// Get a neighboring solution by swapping two jobs in the schedule
Solution getNeighbor(const Solution& currentSolution) {
    Solution neighbor = currentSolution;
    int pos1 = rand() % neighbor.schedule.size();
    int pos2 = rand() % neighbor.schedule.size();
    swap(neighbor.schedule[pos1], neighbor.schedule[pos2]);
    neighbor.makespan = calculateMakespan(neighbor.schedule);
    return neighbor;
}

// Main Tabu Search function
Solution tabuSearch() {
    Solution currentSolution = generateInitialSolution();
    Solution bestSolution = currentSolution;

    queue<pair<int, int> > tabuList;  // Added space between right angle brackets

    // Tabu Search loop
    for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration) {
        Solution bestNeighbor = currentSolution;
        
        // Explore neighbors
        for (int i = 0; i < numJobs; ++i) {
            Solution neighbor = getNeighbor(currentSolution);
            if (neighbor.makespan < bestNeighbor.makespan) {
                bestNeighbor = neighbor;
            }
        }

        // Check if the move is tabu
        bool isTabu = false;
        queue<pair<int, int> > tempTabuList = tabuList;  // Added space between right angle brackets
        while (!tempTabuList.empty()) {
            pair<int, int> move = tempTabuList.front();
            tempTabuList.pop();
            if ((move.first == bestNeighbor.schedule[0] && move.second == bestNeighbor.schedule[1]) ||
                (move.first == bestNeighbor.schedule[1] && move.second == bestNeighbor.schedule[0])) {
                isTabu = true;
                break;
            }
        }

        // Update solution if not tabu or if better than the best known solution
        if (!isTabu || bestNeighbor.makespan < bestSolution.makespan) {
            currentSolution = bestNeighbor;

            // Update tabu list
            if (tabuList.size() >= TABU_TENURE) {
                tabuList.pop();
            }
            tabuList.push(make_pair(currentSolution.schedule[0], currentSolution.schedule[1]));
        }

        // Update the best solution found
        if (currentSolution.makespan < bestSolution.makespan) {
            bestSolution = currentSolution;
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

    // Initialize random seed
    initializeRandom();

    // Run Tabu Search
    clock_t start = clock();
    Solution bestSolution = tabuSearch();
    clock_t end = clock();

    double duration = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    cout << "Best makespan (fitness): " << bestSolution.makespan << endl;
    cout << "Execution time: " << duration << " ms" << endl;

    return 0;
}
