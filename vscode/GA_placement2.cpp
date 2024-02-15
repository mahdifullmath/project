
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>

using namespace std;

// Server class
class Server {
public:
    int cpu;
    int memory;
    int storage;
    
    Server(int cpu, int memory, int storage) {
        this->cpu = cpu;
        this->memory = memory;
        this->storage = storage;
    }
};

// VM class 
class VM {
public:
    int cpu;
    int memory;
    int storage;
    
    VM(int cpu, int memory, int storage) {
        this->cpu = cpu;
        this->memory = memory;
        this->storage = storage;
    }
};

// Placement class
class Placement {
public:
    vector<int> structure;
    double fitness;

    Placement() {
        fitness = 0.0;
    }

    Placement(const vector<VM>& vms, const vector<Server>& servers) {
        structure.resize(vms.size(), -1);
        fitness = calculateFitness(vms, servers);
    }

   double calculateFitness(const vector<VM>& vms, const vector<Server>& servers) {
        double total_utilization = 0;
        double total_energy = 0;

        int NUM_HOSTS = servers.size();
        int NUM_VMS = vms.size();
        int NUM_RESOURCES = 3; // Assuming 3 resources: CPU, memory, and storage

        for (int i = 0; i < NUM_HOSTS; i++) {
            double host_utilization = 0;
            double host_energy = 0;

            for (int j = 0; j < NUM_VMS; j++) {
                if (structure[i] == j) {
                    host_utilization += (double)vms[j].cpu / servers[i].cpu;
                    host_utilization += (double)vms[j].memory / servers[i].memory;
                    host_utilization += (double)vms[j].storage / servers[i].storage;
                    
                    host_energy += vms[j].cpu * 10; // Assume energy consumption is proportional to CPU usage
                    host_energy += vms[j].memory * 5; // Assume energy consumption is proportional to memory usage
                    host_energy += vms[j].storage * 3; // Assume energy consumption is proportional to storage usage
                }
            }

            total_utilization += host_utilization / NUM_VMS;
            total_energy += host_energy;
        }

        // Fitness is a combination of utilization and energy consumption
        return total_utilization - total_energy;
    }
};


class GeneticAlgorithm {
private:
    int population_size;
    int num_generations;
    int tournament_size;
    double mutation_rate;
    float crossover_rate;
    vector<VM> vms;
    vector<Server> servers;

    vector<Placement> initialize_population() {
        vector<Placement> population;
        for (int i = 0; i < population_size; i++) {
            Placement solution(vms, servers);
            population.push_back(solution);
        }
        return population;
    }

    vector<Placement> elitism(const vector<Placement>& next_generation, int elitism_count) {
        vector<Placement> sorted_population = next_generation;
        sort(sorted_population.begin(), sorted_population.end(), [](const Placement& a, const Placement& b) {
            return a.fitness > b.fitness;
        });
        vector<Placement> elite_population(sorted_population.begin(), sorted_population.begin() + elitism_count);
        return elite_population;
    }

    void perform_tournament_selection(vector<Placement>& population) {
        vector<Placement> next_generation;

        while (next_generation.size() < population.size()) {
            vector<Placement> tournament_contestants;

            // Randomly select contestants for the tournament
            for (int i = 0; i < tournament_size; i++) {
                int contestant_index = rand() % population.size();
                tournament_contestants.push_back(population[contestant_index]);
            }

            // Select the contestant with the highest fitness as the winner
            Placement winner = tournament_contestants[0];
            for (int i = 1; i < tournament_contestants.size(); i++) {
                if (tournament_contestants[i].fitness > winner.fitness) {
                    winner = tournament_contestants[i];
                }
            }

            next_generation.push_back(winner);
        }

        population = next_generation;
    }

    void perform_crossover(vector<Placement>& population) {
    
        vector<Placement> next_generation;

        for (int i = 0; i < population.size(); i += 2) {
            // Select parents for crossover
            Placement parent1 = population[i];
            Placement parent2 = population[i + 1];

            // Perform crossover with a certain probability (crossover_rate)
            if (static_cast<double>(rand()) / RAND_MAX < crossover_rate) {
                // Perform crossover operation (e.g., single-point crossover)
                int crossover_point = rand() % parent1.structure.size();

                Placement child1(vms, servers);
                Placement child2(vms, servers);

                for (int j = 0; j < crossover_point; j++) {
                    child1.structure.push_back(parent1.structure[j]);
                    child2.structure.push_back(parent2.structure[j]);
                }   

                for (int j = crossover_point; j < parent1.structure.size(); j++) {
                    child1.structure.push_back(parent2.structure[j]);
                    child2.structure.push_back(parent1.structure[j]);
                }

                next_generation.push_back(child1);
                next_generation.push_back(child2);
            } 
            else {
                // If crossover doesn't occur, add the parents to the next generation
                next_generation.push_back(parent1);
                next_generation.push_back(parent2);
            }
        }

        population = next_generation;
    }

    

    void perform_mutation(vector<Placement>& population) {
        for (auto& solution : population) {
            // Perform mutation on each individual with a certain probability (mutation_rate)
            if (static_cast<double>(rand()) / RAND_MAX < mutation_rate) {
                // Select a random VM and a random server for mutation
                int vm_index = rand() % solution.structure.size();
                int server_index = rand() % servers.size();

                // Mutate the placement of the selected VM to the randomly chosen server
                solution.structure[vm_index] = server_index;
            }
        }
    }


public:
    GeneticAlgorithm(int pop_size, int num_gen, int tour_size, double mut_rate,float crossover_ratee, const vector<VM>& vm_list, const vector<Server>& server_list)
        : population_size(pop_size), num_generations(num_gen), tournament_size(tour_size), mutation_rate(mut_rate),crossover_rate(crossover_ratee), vms(vm_list), servers(server_list) {}

    vector<vector<int>> run() {
        vector<Placement> population = initialize_population();

        for (int generation = 0; generation < num_generations; generation++) {
            perform_tournament_selection(population);
            perform_crossover(population);
            perform_mutation(population);

            vector<Placement> next_generation = population; // Placeholder for genetic algorithm operations

            population = elitism(next_generation, population_size / 2);
        }

        // Select the best Placement from the final population
        return population[0].structure;
    }
};

int main() {
    srand(time(NULL));

    vector<VM> vms; // Initialize VM objects
    vector<Server> servers; // Initialize Server objects

    GeneticAlgorithm ga(100, 100, 10, 0.1,0.5, vms, servers);
    vector<vector<int>> best_placement = ga.run();

    cout << "Best placement: " << endl;
    // Output the best placement

    double fitness = 0.0; // Calculate the fitness of the best placement
    cout << "Fitness: " << fitness << endl;

    return 0;
}
