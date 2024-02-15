/*
 * Hho.cpp
 *
 *  Created on: Jan 15, 2024
 *      Author: mahdi
 */
#include <cmath>
#include <time.h>
#include <algorithm>
#include <random>
#include <iostream>
#include <vector>
#include "Hho.h"

Hho::Hho()
{
}
Hho::~Hho()
{
}

Hho::Hho(int epoch, int pop_size, int num_dims)
{
	this->epoch = epoch;
	this->pop_size = pop_size;
	this->sort_flag = true;
	this->dimPlacement = num_dims;
	//this->Hhoin(pop_size);
}
void Hho::Hhoin( int pop_size) {

        population.resize(pop_size);
        for(auto& solution : population) {
            solution = Placement();
            random_shuffle(solution.structure.begin(), solution.structure.end());
        }

        bestSolution = population[0];
    }

void Hho:: run( vector<vm> vms,  vector<pm> servers) {
        for(int i = 0; i < Hho_epoch; ++i) {
            scoutPhase(vms, servers);
            explorePhase(vms, servers);
            exploitPhase(vms, servers);
            attackPhase(vms, servers);
            updateBest();
        }
    }

void Hho:: scoutPhase( vector<vm> vms,  vector<pm> servers) {

		
	/*
        for(auto& solution : population) {
            for(auto& placement : solution.structure) {
                if(placement.id < 0 && placement.id >= MAX_NUM_PMS) {
                    placement.id = generator() % MAX_NUM_PMS;
                }
            }
        }*/
    }

void Hho::explorePhase( vector<vm> vms,  vector<pm> servers) {
        for(auto& solution : population) {
            double fit = solution.fit;

            for(int i = 0; i < solution.structure.size(); ++i) {
                vector<int> neighbors;

                for(int j = 0; j < servers.size(); ++j) {
                    if(j != solution.structure[i].id) {
                        neighbors.push_back(j);
                    }
                }

                if(!neighbors.empty()) {
                    int index = generator() % neighbors.size();
                    int neighbor = neighbors[index];
                    solution.structure[i].id = neighbor;
                }
            }
        }
    }
void Hho::exploitPhase( vector<vm> vms,  vector<pm> servers) {
        for (auto& solution : population) {
            for (int i = 0; i < solution.structure.size(); ++i) {
                auto& placement = solution.structure[i];
                if (placement.id >= 0 && placement.id < MAX_NUM_PMS) {
                    pm server = servers[placement.id];
                    vector<pm> bestServers;
                    for (int j = 0; j < MAX_NUM_PMS; ++j) {
                        if (j != placement.id) {
                            pm neighborServer = servers[j];
                            if (neighborServer.util.cpu >= vms[i].util.cpu && neighborServer.util.mem >= vms[i].util.mem && neighborServer.util.bw >= vms[i].util.bw) {
                                bestServers.push_back(servers[j]);
                            }
                        }
                    }
                    if (!bestServers.empty()) {
                        int index = static_cast<int>(generator() % bestServers.size());
                        placement.id = bestServers[index].id;
                    }
                }
            }
            solution.calculateFitPlacement( servers,vms);
        }
    }

void Hho::attackPhase( vector<vm> vms,  vector<pm> servers) {
        vector<Placement> sortedPopulation = population;
        sort(sortedPopulation.begin(), sortedPopulation.end(),
            [](const Placement& a, const Placement& b) {
                return a.fit > b.fit;
            });

        for (auto& solution : population) {
            uniform_real_distribution<double> dist(0.0, 1.0);
            double moveProb = dist(generator);
            if (moveProb > 0.5) {
                int currentVM = rand() % NUM_SERVICES;
                unsigned int currentServer = solution.structure[currentVM].id;
                double p = dist(generator);
                if (p > 0.5) {
                    // Move towards the global best
                    solution.structure[currentVM] = bestSolution.structure[currentVM];
                } else {
                    // Move towards the personal best
                    unsigned int personalBestServer = 0;
                    double personalBestFitness = 0.0;
                    for (int j = 0; j < sortedPopulation.size(); ++j) {
                        if (sortedPopulation[j].fit > personalBestFitness) {
                            personalBestFitness = sortedPopulation[j].fit;
                            personalBestServer = sortedPopulation[j].structure[currentVM].id;
                        }
                    }
                    solution.structure[currentVM].id = personalBestServer;
                }
                solution.calculateFitPlacement( servers , vms);
            }
        }
    }

void Hho::updateBest() {
        for(auto& solution : population) {
            if(solution.fit > bestSolution.fit) {
                bestSolution = solution;
            }
        }
    }

//
//int randomNM(int start, int end)
//{
//	srand(time_t);
//	int range = (end - start) + 1;
//	int random_int = start + (rand() % range);
//	return random_int;
//}
//
void Hho::initialize(vector<vm> VMList)
{
	// create Placements where each Placement has a structure to hold the VMP solution
	for (unsigned int i = 0; i < pop_size; i++)
	{
		Placement Placement1;
		unsigned int rem_dim_Placement = dimPlacement;
		std::vector<unsigned int> vm_list;
		for (unsigned int i = 0; i < dimPlacement; i++)
		{
			vm_list.push_back(i);
		}
		// std::cout<<"initializing the pop"<<std::endl;
		for (unsigned int j = 0; j < dimPlacement; j++)
		{
			unsigned int Irand = rand() % vm_list.size();
			vmPlacement myVM;
			myVM.id = vm_list[Irand];
			myVM.vmInfo = VMList[Irand];
			Placement1.structure.push_back(myVM);
			// pop_temp[i].push_back(vm_list[Irand]);
			vm_list.erase(vm_list.begin() + Irand);
			Placement1.PlacementID = i;
		}
		pop.push_back(Placement1);
	}
	bestFit = 1e+20;
	worstFit = 0;
	bestStabLevel = 1e+30;
	bestPow_dis = 1e+30;
	bestResourceWastage = 1e+30;
	currentIteration = 0;
	crossOverCount = 0, mutationCount = 0;
	//numFuncEval_Hho = 0;
}

void Hho::printPop()
{
	// std:: vector< std::vector<unsigned int> >::iterator row;
	// std:: vector<unsigned int>::iterator col;
	int i = 0;
	for (std::vector<Placement>::iterator it1 = pop.begin(); it1 != pop.end(); ++it1)
	{
		std::cout << "Placement-" << it1->PlacementID << ":";
		for (std::vector<vmPlacement>::iterator it2 = it1->structure.begin(); it2 != it1->structure.end(); ++it2)
		{
			std::cout << " " << it2->id;
		}
		i++;
		std::cout << std::endl;
	}
}
//// TODO: assign a proper and valid ID to each newly created Placement
//void Hho::printPlacementVector(vector<Placement> vectPlacements)
//{
//	cout << endl;
//	for (std::vector<Placement>::iterator it1 = vectPlacements.begin(); it1 != vectPlacements.end(); ++it1)
//	{
//		std::cout << "Placement-" << it1->PlacementID << ":";
//		for (std::vector<vmGene>::iterator it2 = it1->structure.begin(); it2 != it1->structure.end(); ++it2)
//		{
//			std::cout << " " << it2->id;
//		}
//		std::cout << std::endl;
//	}
//}
//
//void Hho::calculateFitnessPop(vector<pm> PMList, vector<vm> VMList)
//{
//	int i = 0;
//	for (auto xx : pop)
//	{
//		xx.calculateFitPlacement(PMList, VMList);
//		if (xx.fit < bestFit)
//		{
//			bestPlacement = xx;
//			bestFit = xx.fit;
//		}
//		if (xx.fit > worstFit)
//		{
//			worstPlacement = xx;
//			worstFit = xx.fit;
//		}
//		if (VERBOSE)
//			cout << "Atm-" << xx.PlacementID << " is " << xx.fit << endl;
//		this->pop[i].fit = xx.fit;
//		i++;
//	}
//	if (VERBOSE)
//		cout << endl;
//}
//
//void Hho::sortPopFitnessDecreasing()
//{
//	// NOTE: a higher fitness is worse since power is used as the fitness.
//	int i = 0;
//	for (unsigned int i = 0; i < pop.size(); i++)
//	{
//		for (unsigned int j = i + 1; j < pop.size(); j++)
//		{
//			unsigned int tmp1 = pop[i].fit;
//			unsigned int tmp2 = pop[j].fit;
//
//			if (tmp1 > tmp2)
//			{
//				swap(pop[i], pop[j]);
//			}
//		}
//	}
//}
//
//void Hho::printFitPop()
//{
//	for (std::vector<Placement>::iterator it1 = pop.begin(); it1 != pop.end(); ++it1)
//	{
//		std::cout << "Fit Placement-" << it1->PlacementID << ": " << it1->fit << endl;
//	}
//}
//
void Hho::evolve(vector<pm> PMList, vector<vm> VMList, std::ofstream &fileName, unsigned int iterationForAvg)
{
	char name_of[100];
	ofstream convergence_file_Hho;
	string str_of1;
	string nvmstr = std::to_string(num_vm_experiment);
	string pselstr = std::to_string(EXPERIMENT_P_VALUE_SELECTOR);
	sprintf(name_of, "%s/CONV_%s_%sVMs_%sPvalue.txt", "results", "Hho", nvmstr.c_str(), pselstr.c_str());
	str_of1 = name_of;

	if (iterationForAvg == 0)
	{
		convergence_file_Hho.open(name_of, std::ofstream::out | std::ofstream::trunc);
		convergence_file_Hho.close();
	}
	convergence_file_Hho.open(str_of1, fstream::app);
	std::vector<std::vector<double>> pop_new;
	vector<Placement> newPop;
	unsigned int cnt_print = 0;
	currentIteration = 0;
	while (currentIteration < Hho_epoch and numFuncEval_ga <= max_FE_ga)
	{ // unsigned int idx = 0; idx < this->epoch; idx++) {
		// calculate fitness of all Placements
		// for(std::vector<Placement>::iterator it1 = pop.begin(); it1 != pop.end(); ++it1){
		newPop.erase(newPop.begin(), newPop.end()); // newPop is formed during each iteration
		// cout<<"best / worst fits"<<bestFit<<" / "<<worstFit<<endl;
		this->calculateFitnessPop(PMList, VMList);
		this->sortPopFitnessDecreasing();
		if (VERBOSE)
			this->printFitPop();
		this->modifyPlacementIndices();
		// cout<<"best / worst fits"<<bestFit<<" / "<<worstFit<<endl;
		// this->setBestAndWorstPlacements();
		// cout<<"best / worst fits"<<bestFit<<" / "<<worstFit<<endl;
		convergence_file_Hho << currentIteration << "  " << bestFit << endl;
		if (VERBOSE)
			printPop();
		// cout<<"Best fit in iteration-"<<idx<<": "<<bestFit<<endl;

		int s = (10 * pop_size) / 100;
		for (int i = 0; i < s; i++)
			newPop.push_back(pop[i]);

		// From 50% of fittest population, Placements
		// will mate to produce offspring
		s = pop_size - s; //(90*pop_size)/100;
//		for (int i = 0; i < s; i++)
//		{
//			//   int len = population.size();
//			int r = randomNM(0, pop.size() - 1);
//			Placement parent1 = pop[r];
//			r = randomNM(0, pop.size() - 1);
//			Placement parent2 = pop[r];
//			//Placement offspring = CrossOver(parent1, parent2, PMList, VMList);
//			//newPop.push_back(offspring);
//		}
//		newPop = mutate(newPop);
		this->run(VMList, PMList);
		currentIteration++;
		cnt_print++;
		// Hho_convergence << cnt<<","<<pop[0].fit<<"\n";
		cout << currentIteration << "...";
		if (cnt_print == 100)
		{
			cout << endl;
			cnt_print = 0;
		}

		if (VERBOSE)
			printPlacementVector(addedPlacementsIteration);
		pop.clear();
		// Placements added/removed in alpha/beta/Hhomma reaction are now added/removed to pop
		// note: in Positron, the worst Placements are merged and added to pop while the worst Placements are removed
		if (VERBOSE)
			printPlacementVector(addedPlacementsIteration);
		for (auto z : addedPlacementsIteration)
			pop.push_back(z);
		addedPlacementsIteration.clear(); //(addedPlacementsIteration.begin(),addedPlacementsIteration.end());
		/* for(auto z:toBeRemovedPlacementsIteration){
			unsigned int idxPlacement=findPlacementIDxInPop(z);
			pop.erase(pop.begin()+idxPlacement);
		}*/
		this->modifyPlacementIndices();
		toBeRemovedPlacementsIteration.clear(); //(toBeRemovedPlacementsIteration.begin(),toBeRemovedPlacementsIteration.end());
		cout << "Pop size:" << pop.size() << endl;
	}
	float power = bestPlacement.calculatePowerPlacement(PMList, VMList);

	Solution = bestPlacement.fitPlacementSequenceUsingFF(PMList, VMList);
	convergence_file_Hho.close();
	// Solution.numActivePMs=num_act_pms;
	Solution.power = power;
	Solution.numVMs = VMList.size();
	// Solution.calculateMappingPower(PMList);
	Solution.calculateMappingWastageAndBalance(PMList);
	// cout<<"Out LB CPU: "<<Solution.loadBalanceMEM<<endl;
	//  cout<<"Out LB MEM: "<<Solution.loadBalanceCPU<<endl;
	//  Solution.calculateMappingLodaBalance(PMList);
	Solution.iterForAvg = iterationForAvg;
	Solution.printMappingStatsIntoFile(fileName, PMList);
}

vector<Placement> Hho::sortPopByFitnessDecreasingly()
{
	vector<Placement> sortedPop;
	sortedPop = pop;
	for (unsigned int i = 0; i < sortedPop.size(); i++)
	{
		for (unsigned int j = i + 1; j < sortedPop.size(); j++)
		{
			float temp1 = sortedPop[i].fit;
			float temp2 = sortedPop[j].fit;
			if (temp1 < temp2)
			{
				swap(sortedPop[i], sortedPop[j]);
			}
		}
	}
	return sortedPop;
}

unsigned int Hho::findPlacementIdInPop(Placement thePlacement)
{
	unsigned int idx = 0;
	for (auto x : pop)
	{
		if (x.PlacementID == thePlacement.PlacementID)
		{
			return idx;
		}
		idx++;
	}
	cout << endl
		 << "ATOM INDEX(" << thePlacement.PlacementID << ") NOT FOUND!!!, CHECK IT" << endl;
	return 0;
}

unsigned int Hho::findValidPlacementID()
{
	vector<unsigned int> allPlacementIndices(pop.size() + 1000);
	vector<unsigned int> missingPlacementID;
	iota(allPlacementIndices.begin(), allPlacementIndices.end(), 0);
	for (auto x : allPlacementIndices)
	{
		bool exists = false;
		for (auto y : pop)
		{
			if (y.PlacementID == x)
			{
				exists = true;
				break;
			}
		}
		for (auto y : addedPlacementsIteration)
		{
			if (y.PlacementID == x)
			{
				exists = true;
				break;
			}
		}
		if (!exists)
		{
			return x;
		}
	}
	cout << "ERRO: no proper Placement ID found!!";
	return 0;
}

void Hho::updateBestPlacement(Placement theNewPlacement)
{
	if (bestFit > theNewPlacement.fit)
	{
		bestPlacement = theNewPlacement;
		bestFit = theNewPlacement.fit;
	}
}

void Hho::updateWorstPlacement(Placement theNewPlacement)
{
	if (worstFit < theNewPlacement.fit)
	{
		worstPlacement = theNewPlacement;
		worstFit = theNewPlacement.fit;
	}
}

void Hho::modifyPlacementIndices()
{
	unsigned int PlacementID = 0;
	for (auto x : pop)
	{
		pop[PlacementID].PlacementID = PlacementID;
		PlacementID++;
	}
}

void Hho::setBestAndWorstPlacements()
{
	float fitArray[pop.size()];
	unsigned int fitIndexInPop[pop.size()];
	unsigned int i = 0;
	for (auto x : pop)
	{
		// updateBestPlacement(x);
		// updateWorstPlacement(x);
		fitArray[i] = x.fit;
		fitIndexInPop[i] = i; // hold i-th index in Pop instead of PlacementID
		i++;
	}

	for (unsigned int ii = 0; ii < pop.size(); ii++)
	{
		for (unsigned int jj = 0; jj < pop.size(); jj++)
		{
			if (fitArray[ii] < fitArray[jj])
			{
				float fitTemp = fitArray[ii];
				fitArray[ii] = fitArray[jj];
				fitArray[jj] = fitTemp;
				fitIndexInPop[ii] = fitIndexInPop[jj];
			}
		}
	}
	/* cout<<"Fit values:"<<endl;
	 for(unsigned int ii=0;ii<pop.size();ii++){
		 cout<<" "<<fitArray[ii];
	 }
	 cout<<endl;*/
	worstPlacement = pop.at(fitIndexInPop[pop.size() - 1]);
	worstFit = fitArray[pop.size() - 1];
	bestPlacement = pop.at(fitIndexInPop[0]);
	bestFit = fitArray[0];
}

void Hho::calculateFitnessPop(vector<pm> PMList, vector<vm> VMList)
{
	int i = 0;
	for (auto xx : pop)
	{
		xx.calculateFitPlacement(PMList, VMList);
		if (xx.fit < bestFit)
		{
			bestPlacement = xx;
			bestFit = xx.fit;
		}
		if (xx.fit > worstFit)
		{
			worstPlacement = xx;
			worstFit = xx.fit;
		}
		if (VERBOSE)
			cout << "Atm-" << xx.PlacementID << " is " << xx.fit << endl;
		this->pop[i].fit = xx.fit;
		i++;
	}
	if (VERBOSE)
		cout << endl;
}

void Hho::sortPopFitnessDecreasing()
{
	// NOTE: a higher fitness is worse since power is used as the fitness.
	int i = 0;
	for (unsigned int i = 0; i < pop.size(); i++)
	{
		for (unsigned int j = i + 1; j < pop.size(); j++)
		{
			unsigned int tmp1 = pop[i].fit;
			unsigned int tmp2 = pop[j].fit;

			if (tmp1 > tmp2)
			{
				swap(pop[i], pop[j]);
			}
		}
	}
}

;

///===========Hho
/*vector<int> createGnome()
{
	int len = num_vm_experiment;
	vector <int> gnome;
	vector<int> list_vm;
	for(int i=0;i<num_vm_experiment;i++) list_vm.push_back(i);
	int num_remaining=num_vm_experiment;
	for(int i=0;i<num_vm_experiment;i++){
		int rand_num=rand()%num_remaining;
		gnome.push_back(list_vm[rand_num]);
		list_vm.erase(list_vm.begin()+rand_num);
		num_remaining--;
		}
	return gnome;
}*/
/*

// Overloading < operator
bool operator<(const Placement &ind1, const Placement &ind2)
{
	return ind1.fit < ind2.fit;
}
float res_wastage_ffd_in_Hho=0;
int num_active_pm_ffd_in_Hho=0;

*/

/*

// Driver code
pair<double,double> main_Hho(vector<pm> pm_set,vector<vm> vm_set)
{
	srand((unsigned)(time(0)));
	for(int i=0;i<num_vm_experiment;i++) Placements[i]=i;
	ofstream Hho_convergence;
	Hho_convergence.open ("conv_Hho.csv");
	Hho_convergence << "iteration,fitness\n";
	// current generation
	int generation = 0;

	vector<Placement> population;
	bool found = false;
	vector<vm> VMlist2;
	ffd ffd1(pm_set,vm_set,  VMlist2);
	pow_ffd_in_Hho=ffd1.pow_dis;
	// create initial population
	 cout<<endl<<"gnome:"<<endl;
	for(int i = 0;i<POPULATION_SIZE_Hho;i++)
	{
		vector<int> gnome = create_gnome();
		//for(int j=0;j<num_vm_experiment;j++) cout<<" "<<gnome[j];
	   // cout<<endl;
		population.push_back(Placement(gnome,pm_set,vm_set));
	}
	int cnt=0;
	int cnt_print=0;
	while(cnt<limit_Hho and FuncEval_Hho<=max_FE_Hho)
	{
		// sort the population in increasing order of fitness score
		sort(population.begin(), population.end());

		// if the Placement having lowest fitness score ie.
		// 0 then we know that we have reached to the target
		// and break the loop

		// Otherwise generate new offsprings for new generation
		vector<Placement> new_generation;

		// Perform Elitism, that mean 10% of fittest population
		// goes to the next generation
		int s = (10*POPULATION_SIZE_Hho)/100;
		for(int i = 0;i<s;i++)
			new_generation.push_back(population[i]);

		// From 50% of fittest population, Placements
		// will mate to produce offspring
		s = (90*POPULATION_SIZE_Hho)/100;
		for(int i = 0;i<s;i++)
		{
		 //   int len = population.size();
			int r = randomNM(0, 50);
			Placement parent1 = population[r];
			r = randomNM(0, 50);
			Placement parent2 = population[r];
			Placement offspring = parent1.mate(parent2, pm_set,  vm_set);
			new_generation.push_back(offspring);
		}
		new_generation=mutate(new_generation);
		population = new_generation;
	 //   cout<< "Generation: " << generation << "\t";
	//    cout<< "Gnome: "<< endl;
	//    for(int i=0;i<num_vm_experiment;i++) cout<<population[0].Placement[i] <<" ";
	//    cout<< endl<<"Fitness: "<< population[0].fitness << "\n";

		generation++;
		cnt++;
		cnt_print++;
		Hho_convergence << generation<<","<<population[0].fit<<"\n";
		cout<<generation<<"...";
		if(cnt_print==100){
				cout <<endl;
				cnt_print=0;
			   }
	 }
	Hho_convergence.close();
	pair<double,double> ret_pair;
	ret_pair=calculate_stat_Hho( population[0],pm_set,vm_set);
	 cout<<endl<< "Generation: " << generation << "\t";
   // cout<< "String: "<< population[0].Placement[0] <<"\t";
	cout<< "Fitness: "<< population[0].fit << "\n";
	cout<<endl<< "NUM of Functional Evaluations: " << FuncEval_Hho;
	return ret_pair;
}
*/
