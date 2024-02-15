// virtual machine placement in cloud data centers:

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

// Define resources
struct Resource {
	int cpu;
	int memory;
	int storage;
};

// Define the virtual machine placement solution
struct Placement {
	int vm_id;
	int server_id;
};

// Define the objective function to evaluate the fitness of a placement solution
double objective_function(const vector<Placement>& placement, const vector<Resource>& resources, const vector<int>& vm_cpu, const vector<int>& vm_memory, const vector<int>& vm_storage) {
	double fitness = 0.0;
	for (int i = 0; i < placement.size(); i++) {
    	int vm_id = placement[i].vm_id;
    	int server_id = placement[i].server_id;
    	if (server_id >= 0 && server_id < resources.size()) {
        	if (vm_cpu[vm_id] <= resources[server_id].cpu && vm_memory[vm_id] <= resources[server_id].memory && vm_storage[vm_id] <= resources[server_id].storage) {
            	fitness += 1.0 / (1.0 + abs(vm_cpu[vm_id] - resources[server_id].cpu) + abs(vm_memory[vm_id] - resources[server_id].memory) + abs(vm_storage[vm_id] - resources[server_id].storage));
        	}
    	}
	}
	return fitness;
}

// Define the constraints to ensure that the placement satisfies the resource requirements and availability
bool constraints(const vector<Placement>& placement, const vector<Resource>& resources, const vector<int>& vm_cpu, const vector<int>& vm_memory, const vector<int>& vm_storage) {
	for (int i = 0; i < placement.size(); i++) {
    	int vm_id = placement[i].vm_id;
    	int server_id = placement[i].server_id;
    	if (server_id >= 0 && server_id < resources.size()) {
        	if (vm_cpu[vm_id] > resources[server_id].cpu || vm_memory[vm_id] > resources[server_id].memory || vm_storage[vm_id] > resources[server_id].storage) {
            	return false;
        	}
    	}
	}
	return true;
}

// Define the Harris Hawk Optimization algorithm
vector<Placement> harris_hawk_optimization(const vector<Resource>& resources, const vector<int>& vm_cpu, const vector<int>& vm_memory, const vector<int>& vm_storage, int population_size, int max_iterations) {
	// Initialize the population of virtual machines
	vector<vector<Placement>> population(population_size);
	for (int i = 0; i < population_size; i++) {
    	vector<Placement> placement(vm_cpu.size());
    	for (int j = 0; j < vm_cpu.size(); j++) {
        	placement[j].vm_id = j;
        	placement[j].server_id = -1;
    	}
    	random_shuffle(placement.begin(), placement.end());
    	population[i] = placement;
	}

	// Initialize the best solution found so far
	vector<Placement> best_solution(vm_cpu.size());
	double best_fitness = 0.0;

	// Start the Harris Hawk Optimization algorithm
	for (int iteration = 0; iteration < max_iterations; iteration++) {
    	// Scouting phase: random
    	for (int i = 0; i < population_size; i++) {
        	for (int j = 0; j < vm_cpu.size(); j++) {
            	if (population[i][j].server_id < 0 || population[i][j].server_id >= resources.size()) {
                	int server_id = rand() % resources.size();
                	population[i][j].server_id = server_id;
            	}
        	}
    	}

    	// Exploring phase
    	for (int i = 0; i < population_size; i++) {
        	double fitness = objective_function(population[i], resources, vm_cpu, vm_memory, vm_storage);
        	for (int j = 0; j <vm_cpu.size(); j++) {
            	if (population[i][j].server_id >= 0 && population[i][j].server_id < resources.size()) {
                	vector<int> neighbors;
                	for (int k = 0; k < resources.size(); k++) {
                    	if (k != population[i][j].server_id) {
                        	vector<Placement> neighbor = population[i];
                        	neighbor[j].server_id = k;
                        	if (constraints(neighbor, resources, vm_cpu, vm_memory, vm_storage)) {
                            	double neighbor_fitness = objective_function(neighbor, resources, vm_cpu, vm_memory, vm_storage);
                            	if (neighbor_fitness > fitness) {
                                	neighbors.push_back(k);
                            	}
                        	}
                    	}
                	}
                	if (!neighbors.empty()) {
                    	int neighbor_id = neighbors[rand() % neighbors.size()];
                    	population[i][j].server_id = neighbor_id;
                	}
            	}
        	}
    	}

    	// Exploiting phase: virtual machines intensify their search around the best resources found so far
    	for (int i = 0; i < population_size; i++) {
        	double fitness = objective_function(population[i], resources, vm_cpu, vm_memory, vm_storage);
        	vector<int> best_servers;
        	for (int j = 0; j < resources.size(); j++) {
            	vector<Placement> neighbor = population[i];
            	for (int k = 0; k < vm_cpu.size(); k++) {
                	if (neighbor[k].server_id == j) {
                    	neighbor[k].server_id = rand() % resources.size();
                	}
            	}
            	if (constraints(neighbor, resources, vm_cpu, vm_memory, vm_storage)) {
                	double neighbor_fitness = objective_function(neighbor, resources, vm_cpu, vm_memory, vm_storage);
                	if (neighbor_fitness > fitness) {
                    	best_servers.push_back(j);
                	}
            	}
        	}
        	if (!best_servers.empty()) {
            	int best_server_id = best_servers[rand() % best_servers.size()];
            	for (int j = 0; j < vm_cpu.size(); j++) {
                	if (population[i][j].server_id == best_server_id) {
                    	population[i][j].server_id = rand() % resources.size();
                	}
            	}
        	}
    	}

    	// Attacking phase: virtual machines collaborate to capture the prey by sharing information about the best resources
    	for (int i = 0; i < population_size; i++) {
        	double fitness = objective_function(population[i], resources, vm_cpu, vm_memory, vm_storage);
        	vector<int> best_servers;
        	for (int j = 0; j < resources.size(); j++) {
            	vector<Placement> neighbor = population[i];
            	for (int k = 0; k < vm_cpu.size(); k++) {
                	if (neighbor[k].server_id == j) {
                    	neighbor[k].server_id = rand() % resources.size();
                	}
            	}
            	if (constraints(neighbor, resources, vm_cpu, vm_memory, vm_storage)) {
                	double neighbor_fitness = objective_function(neighbor, resources, vm_cpu, vm_memory, vm_storage);
                	if (neighbor_fitness > fitness) {
                    	best_servers.push_back(j);
                	}
            	}
        	}
        	if (!best_servers.empty()) {
            	int best_server_id = best_servers[rand() % best_servers.size()];
            	for (int j = 0; j < vm_cpu.size(); j++) {
                	if (population[i][j].server_id == best_server_id) {
                    	population[i][j].server_id = rand() % resources.size();
                	}
            	}
        	}
    	}

    	// Update the bestsolution found so far
    	for (int i = 0; i < population_size; i++) {
        	double fitness = objective_function(population[i], resources, vm_cpu, vm_memory, vm_storage);
        	if (fitness > best_fitness) {
            	best_solution = population[i];
            	best_fitness = fitness;
        	}
    	}
	}

	return best_solution;
}


int main() {
	// Define the cloud data center resources
	vector<Resource> resources = {{100, 100, 100}, {200, 200, 200}, {300, 300, 300}};

	// Define the virtual machine resource requirements
	vector<int> vm_cpu = {50, 100, 150};
	vector<int> vm_memory = {50, 100, 150};
	vector<int> vm_storage = {50, 100, 150};

	// Run Harris Hawk Optimization to find the best virtual machine placement solution
	vector<Placement> placement = harris_hawk_optimization(resources, vm_cpu, vm_memory, vm_storage, 10, 100);

	// Print the best virtual machine placement solution found
	cout << "Best virtual machine placement solution found:" << endl;
	for (int i = 0; i < placement.size(); i++) {
    	cout << "VM " << placement[i].vm_id << " placed on server " << placement[i].server_id << endl;
	}

	return 0;
}

