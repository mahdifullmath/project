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

    
    Placement() {
        fitness = 0.0;
    }

    Placement(const vector<vm>& vms, const vector<Server>& servers) {
        structure.resize(vms.size(), -1);
        fitness = calculateFitness(vms, servers);
    }

    double calculateFitness(const vector<vm>& vms, const vector<Server>& servers) {
        vector<Server> modifiableServers = servers;
        double fit = 0.0;
        vector<double> server_utilization(servers.size(), 0.0);

        // Calculate total utilization and total slack for each server
        for(int i = 0; i < structure.size(); ++i) {
            int vmId = i;
            int serverId = structure[i];
            if(serverId >= 0 && serverId < servers.size()) {
                VM vm = vms[vmId]; 
                Server& server = modifiableServers[serverId];

                if(vm.cpu <= server.cpu && vm.memory <= server.memory && vm.storage <= server.storage) {
                    server.cpu -= vm.cpu;
                    server.memory -= vm.memory;
                    server.storage -= vm.storage;
                    server_utilization[serverId] += vm.cpu + vm.memory + vm.storage;
                }
            }
        }

        // Calculate the fitness score based on server utilization
        for(int serverId = 0; serverId < servers.size(); ++serverId) {
            Server& server = modifiableServers[serverId];
            double total_capacity = server.cpu + server.memory + server.storage;

            // Server utilization percentage
            double utilization = server_utilization[serverId] / total_capacity;

            // Use the utilization in the fitness function
            fit += utilization;  // Higher utilization should contribute to a higher fitness value
        }

        // Normalize the fitness based on the number of servers
        return fit / servers.size();
    }
    
    bool satisfiesConstraints(const vector<VM>& vms, const vector<Server>& servers) {
        for(int i = 0; i < structure.size(); ++i) {
            int vmId = i; 
            int serverId = structure[i];
            if(serverId >= 0 && serverId < servers.size()) {
                VM vm = vms[vmId];
                Server server = servers[serverId];
                if(vm.cpu > server.cpu || vm.memory > server.memory || vm.storage > server.storage) {
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

    Hho( int pop_size) {
        population.resize(pop_size);
        for(auto& solution : population) {
            solution = Placement();
            random_shuffle(solution.structure.begin(), solution.structure.end());
        }
        
        bestSolution = population[0];
    }

    void run(int maxIterations,const vector<vm>& vms, const vector<pm>& servers) {
        for(int i = 0; i < maxIterations; ++i) {
            scoutPhase(vms, servers);  
            explorePhase(vms, servers);
            exploitPhase(vms, servers);
            attackPhase(vms, servers);  
            updateBest();
        }
    }

private:
    private:
    void scoutPhase(const vector<vm>& vms, const vector<pm>& servers) {
        for(auto& solution : population) {
            for(auto& placement : solution.structure) {
                if(placement < 0 && placement >= servers.size()) {
                    placement = generator() % servers.size(); 
                }
            }
        }
    }

    void explorePhase(const vector<VM>& vms, const vector<pm>& servers) {
        for(auto& solution : population) {
            double fit = solution.fitness;
            
            for(int i = 0; i < solution.structure.size(); ++i) {
                vector<int> neighbors;
                
                for(int j = 0; j < servers.size(); ++j) {
                    if(j != solution.structure[i]) {
                        neighbors.push_back(j);
                    }
                }
                
                if(!neighbors.empty()) {
                    int index = generator() % neighbors.size();
                    int neighbor = neighbors[index]; 
                    solution.structure[i] = neighbor;
                } 
            }
        }
    }

        void exploitPhase(const vector<vm>& vms, const vector<pm>& servers) {
        for (auto& solution : population) {
            for (int i = 0; i < solution.structure.size(); ++i) {
                auto& placement = solution.structure[i];
                if (placement >= 0 && placement < servers.size()) {
                    pm server = servers[placement];
                    vector<int> bestServers;
                    for (int j = 0; j < servers.size(); ++j) {
                        if (j != placement) {
                            pm neighborServer = servers[j];
                            if (neighborServer.cpu >= vms[i].cpu && neighborServer.memory >= vms[i].memory && neighborServer.storage >= vms[i].storage) {
                                bestServers.push_back(j);
                            }
                        }
                    }
                    if (!bestServers.empty()) {
                        int index = static_cast<int>(generator() % bestServers.size());
                        placement = bestServers[index];
                    }
                }
            }
            solution.fitness = solution.calculateFitness(vms, servers);
        }
    }

 void attackPhase(const vector<vm>& vms, const vector<pm>& servers) {
        vector<Placement> sortedPopulation = population;
        sort(sortedPopulation.begin(), sortedPopulation.end(), 
            [](const Placement& a, const Placement& b) {
                return a.fitness > b.fitness;
            });

        for (auto& solution : population) {
            uniform_real_distribution<double> dist(0.0, 1.0);
            double moveProb = dist(generator);
            if (moveProb > 0.5) {
                int currentVM = rand() % vms.size();
                int currentServer = solution.structure[currentVM];
                double p = dist(generator);
                if (p > 0.5) {
                    // Move towards the global best
                    solution.structure[currentVM] = bestSolution.structure[currentVM];
                } else {
                    // Move towards the personal best
                    int personalBestServer = -1;
                    double personalBestFitness = 0.0;
                    for (int j = 0; j < sortedPopulation.size(); ++j) {
                        if (sortedPopulation[j].fitness > personalBestFitness) {
                            personalBestFitness = sortedPopulation[j].fitness;
                            personalBestServer = sortedPopulation[j].structure[currentVM];
                        }
                    }
                    solution.structure[currentVM] = personalBestServer;
                }
                solution.fitness = solution.calculateFitness(vms, servers);
            }
        }
    }
   
    void updateBest() {
        for(auto& solution : population) {
            if(solution.fitness > bestSolution.fitness) {
                bestSolution = solution;
            }
        }
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
    hho.run(100, vms, servers);

    // Print best placement
    return 0;
}