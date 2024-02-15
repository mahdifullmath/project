
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>

using namespace std;

// Define the problem space
const int NUM_VMS = 10;
const int NUM_HOSTS = 5;
const int NUM_RESOURCES = 3;
const int RESOURCE_CAPACITIES[NUM_RESOURCES] = {100, 200, 300};

// Define the fitness function
double fitness(vector<vector<int>> placement) {
	double total_utilization = 0;
	double total_energy = 0;
	for (int i = 0; i < NUM_HOSTS; i++) {
    	double host_utilization = 0;
    	double host_energy = 0;
    	for (int j = 0; j < NUM_VMS; j++) {
        	if (placement[i][j] == 1) {
            	for (int k = 0; k < NUM_RESOURCES; k++) {
                	host_utilization += placement[i][j] * RESOURCE_CAPACITIES[k];
            	}
            	host_energy += placement[i][j] * 10; // Assume energy consumption is proportional to resource utilization
        	}
    	}
    	total_utilization += host_utilization / (NUM_RESOURCES * RESOURCE_CAPACITIES[0]);
    	total_energy += host_energy;
	}
	return total_utilization - total_energy;
}

// Initialize the population
vector<vector<vector<int>>> initialize_population(int population_size) {
	vector<vector<vector<int>>> population;
	for (int i = 0; i < population_size; i++) {
    	vector<vector<int>> placement(NUM_HOSTS, vector<int>(NUM_VMS, 0));
    	for (int j = 0; j < NUM_VMS; j++) {
        	int host_index = rand() % NUM_HOSTS;
        	placement[host_index][j] = 1;
    	}
    	population.push_back(placement);
	}
	return population;
}

// Select parents using tournament selection
vector<vector<int>> tournament_selection(vector<vector<vector<int>>> population, int tournament_size) {
	vector<vector<int>> best_parent;
	double best_fitness = -1000000;
	for (int i = 0; i < tournament_size; i++) {
    	int index = rand() % population.size();
    	vector<vector<int>> candidate = population[index];
    	double candidate_fitness = fitness(candidate);
    	if (candidate_fitness > best_fitness) {
        	best_parent = candidate;
        	best_fitness = candidate_fitness;
    	}
	}
	return best_parent;
}

// Reproduce using crossover
vector<vector<int>> crossover(vector<vector<int>> parent1, vector<vector<int>> parent2) {
	vector<vector<int>> child(NUM_HOSTS, vector<int>(NUM_VMS, 0));
	for (int i = 0; i < NUM_HOSTS; i++) {
    	for (int j = 0; j < NUM_VMS; j++) {
        	if (rand() % 2 == 0) {
            	child[i][j] = parent1[i][j];
        	} else {
            	child[i][j] = parent2[i][j];
        	}
    	}
	}
	return child;
}

// Mutate by randomly moving a VM to a different host
void mutate(vector<vector<int>>& placement, double mutation_rate) {
	for (int i = 0; i < NUM_VMS; i++) {
    	if ((double) rand() / RAND_MAX < mutation_rate) {
        	int old_host_index = -1;
        	int new_host_index = -1;
        	for (int j = 0; j < NUM_HOSTS; j++) {
            	if (placement[j][i] == 1) {
                	old_host_index = j;
                	placement[j][i] = 0;
                	break;
            	}
        	}
        	while (new_host_index == -1 || new_host_index == old_host_index) {
            	new_host_index = rand() % NUM_HOSTS;
        	}
        	placement[new_host_index][i] = 1;
    	}
	}
}

// Select survivors using elitism
vector<vector<vector<int>>> elitism(vector<vector<vector<int>>> population, int num_survivors) {
	vector<pair<double, vector<vector<int>>>> fitness_population;
	for (auto placement : population) {
    	double placement_fitness = fitness(placement);
    	fitness_population.push_back(make_pair(placement_fitness, placement));
	}
	sort(fitness_population.rbegin(), fitness_population.rend());
	vector<vector<vector<int>>> survivors;
	for (int i = 0; i < num_survivors; i++) {
    	survivors.push_back(fitness_population[i].second);
	}
	return survivors;
}

// Run the genetic algorithm
vector<vector<int>> run_genetic_algorithm(int population_size, int num_generations, int tournament_size, double mutation_rate) {
	vector<vector<vector<int>>> population = initialize_population(population_size);
	for (int generation = 0; generation < num_generations; generation++) {
    	vector<vector<vector<int>>> next_generation;
    	for (int i = 0; i < population_size; i++) {
        	vector<vector<int>> parent1 = tournament_selection(population, tournament_size);
        	vector<vector<int>> parent2 = tournament_selection(population, tournament_size);
        	vector<vector<int>> child = crossover(parent1, parent2);
        	mutate(child, mutation_rate);
        	next_generation.push_back(child);
    	}
    	population = elitism(next_generation, population_size / 2);
	}
	vector<pair<double, vector<vector<int>>>> fitness_population;
	for (auto placement : population) {
    	double placement_fitness = fitness(placement);
    	fitness_population.push_back(make_pair(placement_fitness, placement));
	}
	sort(fitness_population.rbegin(), fitness_population.rend());
	return fitness_population[0].second;
}

// Main function
int main() {
	srand(time(NULL));
	vector<vector<int>> best_placement = run_genetic_algorithm(100, 100, 10, 0.1);
	cout << "Best placement: " << endl;
	for (int i = 0; i < NUM_HOSTS; i++) {
    	for (int j = 0; j < NUM_VMS; j++) {
        	cout << best_placement[i][j] << " ";
    	}
    	cout << endl;
	}
	cout << "Fitness: " << fitness(best_placement) << endl;
	return 0;
}
