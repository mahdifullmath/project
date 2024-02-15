#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

class VirtualMachine {
public:
    double cpu;
    double memory;
    double storage;

    VirtualMachine(double cpu, double memory, double storage)
        : cpu(cpu), memory(memory), storage(storage) {}
};

class Server {
public:
    double maxCpu;
    double maxMemory;
    double maxStorage;
    double usedCpu;
    double usedMemory;
    double usedStorage;

    Server(double cpu, double memory, double storage)
        : maxCpu(cpu), maxMemory(memory), maxStorage(storage), usedCpu(0), usedMemory(0), usedStorage(0) {}

    bool canHost(const VirtualMachine& vm) {
        return (usedCpu + vm.cpu) <= maxCpu &&
               (usedMemory + vm.memory) <= maxMemory &&
               (usedStorage + vm.storage) <= maxStorage;
    }

    void host(const VirtualMachine& vm) {
        usedCpu += vm.cpu;
        usedMemory += vm.memory;
        usedStorage += vm.storage;
    }

    double utilizationScore() {
        // Simplified objective score: maximizes utilization while avoiding overutilization
        return (usedCpu / maxCpu) + (usedMemory / maxMemory) + (usedStorage / maxStorage);
    }

    void reset() {
        usedCpu = 0;
        usedMemory = 0;
        usedStorage = 0;
    }
};

class Placement {
public:
    std::vector<int> serverIndices;
    double fitness;

    Placement(int numVMs)
        : serverIndices(numVMs, -1), fitness(0.0) {}
};

class VMPlacementOptimizer {
private:
    int epoch;
    int pop_size;
    std::vector<Placement> population;
    Placement g_best;
    std::vector<Server> servers;
    std::vector<VirtualMachine> vms;
    std::default_random_engine generator;

public:
    VMPlacementOptimizer(int epoch, int pop_size, std::vector<Server>& servers,
                         std::vector<VirtualMachine>& vms)
        : epoch(epoch), pop_size(pop_size), servers(servers), vms(vms) {
        // Initialize the placement solutions
        population.reserve(pop_size);
        for (int i = 0; i < pop_size; i++) {
            population.push_back(Placement(vms.size()));
        }
        g_best = population[0];
    }

    void evaluateFitness(Placement& solution) {
        for (Server& server : servers) {
            server.reset();
        }
        for (size_t i = 0; i < vms.size(); ++i) {
            int serverIdx = solution.serverIndices[i];
            if (serverIdx != -1 && servers[serverIdx].canHost(vms[i])) {
                servers[serverIdx].host(vms[i]);
            } else {
                solution.fitness = -1;
                return;
            }
        }
        // Calculate fitness as the sum of utilization scores
        solution.fitness = std::accumulate(servers.begin(), servers.end(), 0.0,
                                           [](double sum, const Server& s) {
                                               return sum + s.utilizationScore();
                                           });
    }

    bool compareSolutions(const Placement& a, const Placement& b) {
        return a.fitness > b.fitness; // We aim for higher fitness (better utilization)
    }

    void evolve() {
        // HHO evolving procedure; you'll need to adapt the implementation
        // to make server allocations and check fitness using the evaluateFitness method
    }

    void printBestSolution() {
        std::cout << "Best placement fitness: " << g_best.fitness << std::endl;
        for (size_t i = 0; i < g_best.serverIndices.size(); i++) {
            int serverIdx = g_best.serverIndices[i];
            std::cout << "VM " << i << " is placed on Server " << serverIdx
                      << " (CPU: " << vms[i].cpu << ", Memory: " << vms[i].memory << ", Storage: " << vms[i].storage << ")" << std::endl;
        }
    }

    // Other functions would go here ...
};

int main() {
    std::vector<Server> servers = {
        Server(100, 100, 100),
        Server(200, 200, 200),
        Server(300, 300, 300),
    };

    std::vector<VirtualMachine> vms = {
        VirtualMachine(10, 20, 30),
        VirtualMachine(20, 30, 10),
        VirtualMachine(30, 40, 50),
    };

    VMPlacementOptimizer optimizer(10000, 100, servers, vms);
    optimizer.evolve();
    optimizer.printBestSolution();

    return 0;
}