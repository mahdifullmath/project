#include <iostream>
#include <vector>
#include <random>
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
    
    Placement(const vector<VM>& vms, const vector<Server>& servers) {
        structure.resize(vms.size(), -1);
        fitness = calculateFitness(vms, servers);
    }

    double calculateFitness(const vector<VM>& vms, const vector<Server>& servers) {
        double fit = 0.0;
        for(int i = 0; i < structure.size(); ++i) {
            int vmId = i;
            int serverId = structure[i];
            if(serverId >= 0 && serverId < servers.size()) {
                VM vm = vms[vmId]; 
                Server server = servers[serverId];
                if(vm.cpu <= server.cpu && vm.memory <= server.memory && vm.storage <= server.storage) {
                    fit += 1.0 / (1 + abs(vm.cpu - server.cpu) + abs(vm.memory - server.memory) + abs(vm.storage - server.storage));
                }
            }
        }
        return fit;
    }
    
    bool satisfiesConstraints(const vector<VM>& vms, const vector<Server>& servers) {
        for(int i = 0; i < structure.size(); ++i) {
            int vmId = i; 
            int serverId = structure[i];
            if(serverId >= 0 && serverId < servers.size()) {
                VM vm = vms[vmId];
                Server server = servers[serverId];
                if(vm.cpu > server.cpu vm.memory > server.memory vm.storage > server.storage) {
                    return false;
                }
            }
        }
        return true;
    }
};

// HarrisHawkOptim class
class Hho {
public:
    vector<Placement> population;
    Placement bestSolution;
    default_random_engine generator;

    Hho(const vector<VM>& vms, const vector<Server>& servers, int popSize) {
        population.resize(popSize);
        for(auto& solution : population) {
            solution = Placement(vms, servers);
            random_shuffle(solution.structure.begin(), solution.structure.end());
        }
        
        bestSolution = population[0];
    }

    void run(int maxIterations) {
        for(int i = 0; i < maxIterations; ++i) {
            scoutPhase();  
            explorePhase();
            exploitPhase();
            attackPhase();  
            updateBest();
        }
    }

private:
    void scoutPhase() {
        // Implement scout phase
    }

    void explorePhase() {
        // Implement explore phase 
    }

    void exploitPhase() {
        // Implement exploit phase
    }

    void attackPhase() {
        // Implement attack phase
    }
    
    void updateBest() {
        // Update best solution
    }
};

int main() {
    // Servers 
    vector<Server> servers = {
        Server(100, 100, 100),
        Server(200, 200, 200),
        Server(300, 300, 300)
    };
    
    // VMs
    vector<VM> vms = {
        VM(50, 50, 50), 
        VM(100, 100, 100),
        VM(150, 150, 150)
    };

    // Optimization
    Hho hho(vms, servers, 10);
    hho.run(100);

    // Print best placement
    return 0;
}


class Hho {
public:
    // Constructor and other functions

     void run(int maxIterations) {
        for(int i = 0; i < maxIterations; ++i) {
            scoutPhase();
            explorePhase();
            exploitPhase();  
            attackPhase();
            updateBest();
        }
    }

private:
    void scoutPhase() {
        for(auto& solution : population) {
            for(auto& placement : solution.placements) {
                if(placement < 0 placement >= servers.size()) {
                    placement = generator() % servers.size(); 
                }
            }
        }
    }

    void explorePhase() {
        for(auto& solution : population) {
            double fit = solution.fitness;
            
            for(int i = 0; i < solution.placements.size(); ++i) {
                vector<int> neighbors;
                
                for(int j = 0; j < servers.size(); ++j) {
                    if(j != solution.placements[i]) {
                        neighbors.push_back(j);
                    }
                }
                
                if(!neighbors.empty()) {
                    int index = generator() % neighbors.size();
                    int neighbor = neighbors[index]; 
                    solution.placements[i] = neighbor;
                } 
            }
        }
    }

    void exploitPhase() {
        for(auto& solution : population) {
            double fit = solution.fitness;
            vector<int> bestServers;
            
            for(int i = 0; i < servers.size(); ++i) {
                if(solution.isBetterNeighbor(i, servers, vms)) {
                    bestServers.push_back(i);
                }
            }
            
            if(!bestServers.empty()) {
                int serverId = bestServers[generator() % bestServers.size()];
                
                for(int i = 0; i < solution.placements.size(); ++i) {
                    if(solution.placements[i] == serverId) {
                        solution.placements[i] = generator() % servers.size();
                    }
                }
            }
        }
    }

    void attackPhase() {
        // Similar to exploit phase
    }
   
    void updateBest() {
        for(auto& solution : population) {
            if(solution.fitness > bestSolution.fitness) {
                bestSolution = solution;
            }
        }
    } 
};