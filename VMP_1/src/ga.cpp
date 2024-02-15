/*
 * ga.cpp
 *
 *  Created on: Nov 1, 2023
 *      Author: mkiani
 */

/*
 * ga.cpp
 *
 *  Created on: Oct 3, 2023
 *      Author: mkiani
 */
#include <cmath>
#include <algorithm>
#include <random>
#include <iostream>
#include <vector>
#include "ga.h"

ga::ga()
{
}
ga::~ga()
{
}

ga::ga(int epoch, int pop_size, int num_dims)
{
	this->epoch = epoch;
	this->pop_size = pop_size;
	this->sort_flag = true;
	this->dimChromosome = num_dims;
}

int randomNM(int start, int end)
{
	int range = (end - start) + 1;
	int random_int = start + (rand() % range);
	return random_int;
}

void ga::initialize(vector<vm> VMList)
{
	// create Chromosomes where each Chromosome has a structure to hold the VMP solution
	for (unsigned int i = 0; i < pop_size; i++)
	{
		Chromosome Chromosome1;
		unsigned int rem_dim_Chromosome = dimChromosome;
		std::vector<unsigned int> vm_list;
		for (unsigned int i = 0; i < dimChromosome; i++)
		{
			vm_list.push_back(i);
		}
		// std::cout<<"initializing the pop"<<std::endl;
		for (unsigned int j = 0; j < dimChromosome; j++)
		{
			unsigned int Irand = rand() % vm_list.size();
			vmGene myVM;
			myVM.id = vm_list[Irand];
			myVM.vmInfo = VMList[Irand];
			Chromosome1.structure.push_back(myVM);
			// pop_temp[i].push_back(vm_list[Irand]);
			vm_list.erase(vm_list.begin() + Irand);
			Chromosome1.ChromosomeID = i;
		}
		pop.push_back(Chromosome1);
	}
	bestFit = 1e+20;
	worstFit = 0;
	bestStabLevel = 1e+30;
	bestPow_dis = 1e+30;
	bestResourceWastage = 1e+30;
	currentIteration = 0;
	crossOverCount = 0, mutationCount = 0;
	numFuncEval_ga = 0;
}

void ga::printPop()
{
	// std:: vector< std::vector<unsigned int> >::iterator row;
	// std:: vector<unsigned int>::iterator col;
	int i = 0;
	for (std::vector<Chromosome>::iterator it1 = pop.begin(); it1 != pop.end(); ++it1)
	{
		std::cout << "Chromosome-" << it1->ChromosomeID << ":";
		for (std::vector<vmGene>::iterator it2 = it1->structure.begin(); it2 != it1->structure.end(); ++it2)
		{
			std::cout << " " << it2->id;
		}
		i++;
		std::cout << std::endl;
	}
}
// TODO: assign a proper and valid ID to each newly created Chromosome
void ga::printChromosomeVector(vector<Chromosome> vectChromosomes)
{
	cout << endl;
	for (std::vector<Chromosome>::iterator it1 = vectChromosomes.begin(); it1 != vectChromosomes.end(); ++it1)
	{
		std::cout << "Chromosome-" << it1->ChromosomeID << ":";
		for (std::vector<vmGene>::iterator it2 = it1->structure.begin(); it2 != it1->structure.end(); ++it2)
		{
			std::cout << " " << it2->id;
		}
		std::cout << std::endl;
	}
}

void ga::calculateFitnessPop(vector<pm> PMList, vector<vm> VMList)
{
	int i = 0;
	for (auto xx : pop)
	{
		xx.calculateFitChromosome(PMList, VMList);
		if (xx.fit < bestFit)
		{
			bestChromosome = xx;
			bestFit = xx.fit;
		}
		if (xx.fit > worstFit)
		{
			worstChromosome = xx;
			worstFit = xx.fit;
		}
		if (VERBOSE)
			cout << "Atm-" << xx.ChromosomeID << " is " << xx.fit << endl;
		this->pop[i].fit = xx.fit;
		i++;
	}
	if (VERBOSE)
		cout << endl;
}

void ga::sortPopFitnessDecreasing()
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

void ga::printFitPop()
{
	for (std::vector<Chromosome>::iterator it1 = pop.begin(); it1 != pop.end(); ++it1)
	{
		std::cout << "Fit Chromosome-" << it1->ChromosomeID << ": " << it1->fit << endl;
	}
}

void ga::evolve(vector<pm> PMList, vector<vm> VMList, std::ofstream &fileName, unsigned int iterationForAvg)
{
	char name_of[100];
	ofstream convergence_file_ga;
	string str_of1;
	string nvmstr = std::to_string(num_vm_experiment);
	string pselstr = std::to_string(EXPERIMENT_P_VALUE_SELECTOR);
	sprintf(name_of, "%s/CONV_%s_%sVMs_%sPvalue.txt", "results", "ga", nvmstr.c_str(), pselstr.c_str());
	str_of1 = name_of;

	if (iterationForAvg == 0)
	{
		convergence_file_ga.open(name_of, std::ofstream::out | std::ofstream::trunc);
		convergence_file_ga.close();
	}
	convergence_file_ga.open(str_of1, fstream::app);
	std::vector<std::vector<double>> pop_new;
	vector<Chromosome> newPop;
	unsigned int cnt_print = 0;
	currentIteration = 0;
	while (currentIteration < GA_epoch and numFuncEval_ga <= max_FE_ga)
	{ // unsigned int idx = 0; idx < this->epoch; idx++) {
		// calculate fitness of all Chromosomes
		// for(std::vector<Chromosome>::iterator it1 = pop.begin(); it1 != pop.end(); ++it1){
		newPop.erase(newPop.begin(), newPop.end()); // newPop is formed during each iteration
		// cout<<"best / worst fits"<<bestFit<<" / "<<worstFit<<endl;
		this->calculateFitnessPop(PMList, VMList);
		this->sortPopFitnessDecreasing();
		if (VERBOSE)
			this->printFitPop();
		this->modifyChromosomeIndices();
		// cout<<"best / worst fits"<<bestFit<<" / "<<worstFit<<endl;
		// this->setBestAndWorstChromosomes();
		// cout<<"best / worst fits"<<bestFit<<" / "<<worstFit<<endl;
		convergence_file_ga << currentIteration << "  " << bestFit << endl;
		if (VERBOSE)
			printPop();
		// cout<<"Best fit in iteration-"<<idx<<": "<<bestFit<<endl;

		int s = (10 * pop_size) / 100;
		for (int i = 0; i < s; i++)
			newPop.push_back(pop[i]);

		// From 50% of fittest population, Chromosomes
		// will mate to produce offspring
		s = pop_size - s; //(90*pop_size)/100;
		for (int i = 0; i < s; i++)
		{
			//   int len = population.size();
			int r = randomNM(0, pop.size() - 1);
			Chromosome parent1 = pop[r];
			r = randomNM(0, pop.size() - 1);
			Chromosome parent2 = pop[r];
			Chromosome offspring = CrossOver(parent1, parent2, PMList, VMList);
			newPop.push_back(offspring);
		}
		newPop = mutate(newPop);
		currentIteration++;
		cnt_print++;
		// ga_convergence << cnt<<","<<pop[0].fit<<"\n";
		cout << currentIteration << "...";
		if (cnt_print == 100)
		{
			cout << endl;
			cnt_print = 0;
		}

		if (VERBOSE)
			printChromosomeVector(addedChromosomesIteration);
		pop.clear();
		// Chromosomes added/removed in alpha/beta/gamma reaction are now added/removed to pop
		// note: in Positron, the worst Chromosomes are merged and added to pop while the worst Chromosomes are removed
		if (VERBOSE)
			printChromosomeVector(addedChromosomesIteration);
		for (auto z : addedChromosomesIteration)
			pop.push_back(z);
		addedChromosomesIteration.clear(); //(addedChromosomesIteration.begin(),addedChromosomesIteration.end());
		/* for(auto z:toBeRemovedChromosomesIteration){
			unsigned int idxChromosome=findChromosomeIDxInPop(z);
			pop.erase(pop.begin()+idxChromosome);
		}*/
		this->modifyChromosomeIndices();
		toBeRemovedChromosomesIteration.clear(); //(toBeRemovedChromosomesIteration.begin(),toBeRemovedChromosomesIteration.end());
		cout << "Pop size:" << pop.size() << endl;
	}
	float power = bestChromosome.calculatePowerChromosome(PMList, VMList);

	Solution = bestChromosome.fitChromosomeSequenceUsingFF(PMList, VMList);
	convergence_file_ga.close();
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

vector<Chromosome> ga::sortPopByFitnessDecreasingly()
{
	vector<Chromosome> sortedPop;
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

unsigned int ga::findChromosomeIdInPop(Chromosome theChromosome)
{
	unsigned int idx = 0;
	for (auto x : pop)
	{
		if (x.ChromosomeID == theChromosome.ChromosomeID)
		{
			return idx;
		}
		idx++;
	}
	cout << endl
		 << "ATOM INDEX(" << theChromosome.ChromosomeID << ") NOT FOUND!!!, CHECK IT" << endl;
	return 0;
}

unsigned int ga::findValidChromosomeID()
{
	vector<unsigned int> allChromosomeIndices(pop.size() + 1000);
	vector<unsigned int> missingChromosomeID;
	iota(allChromosomeIndices.begin(), allChromosomeIndices.end(), 0);
	for (auto x : allChromosomeIndices)
	{
		bool exists = false;
		for (auto y : pop)
		{
			if (y.ChromosomeID == x)
			{
				exists = true;
				break;
			}
		}
		for (auto y : addedChromosomesIteration)
		{
			if (y.ChromosomeID == x)
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
	cout << "ERRO: no proper Chromosome ID found!!";
	return 0;
}

void ga::updateBestChromosome(Chromosome theNewChromosome)
{
	if (bestFit > theNewChromosome.fit)
	{
		bestChromosome = theNewChromosome;
		bestFit = theNewChromosome.fit;
	}
}

void ga::updateWorstChromosome(Chromosome theNewChromosome)
{
	if (worstFit < theNewChromosome.fit)
	{
		worstChromosome = theNewChromosome;
		worstFit = theNewChromosome.fit;
	}
}

void ga::modifyChromosomeIndices()
{
	unsigned int ChromosomeID = 0;
	for (auto x : pop)
	{
		pop[ChromosomeID].ChromosomeID = ChromosomeID;
		ChromosomeID++;
	}
}

void ga::setBestAndWorstChromosomes()
{
	float fitArray[pop.size()];
	unsigned int fitIndexInPop[pop.size()];
	unsigned int i = 0;
	for (auto x : pop)
	{
		// updateBestChromosome(x);
		// updateWorstChromosome(x);
		fitArray[i] = x.fit;
		fitIndexInPop[i] = i; // hold i-th index in Pop instead of ChromosomeID
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
	worstChromosome = pop.at(fitIndexInPop[pop.size() - 1]);
	worstFit = fitArray[pop.size() - 1];
	bestChromosome = pop.at(fitIndexInPop[0]);
	bestFit = fitArray[0];
}

vector<Chromosome> ga::mutate(vector<Chromosome> popChromosomes)
{
	mutationCount++;
	int mut_num = (int)MUTATION_RATE * dimChromosome * pop_size;
	for (int i = 0; i < mut_num; i++)
	{
		int rnd = rand() % popChromosomes.size();
		Chromosome randChromosome = popChromosomes[rnd];
		int rand1 = rand() % (num_vm_experiment);
		int rand2 = rand() % (num_vm_experiment);
		swap(popChromosomes[rnd].structure[rand1], popChromosomes[rnd].structure[rand2]); // child_chromosome.push_back(mutated_Chromosomes());
	}
	return popChromosomes;
}

// Perform mating and produce new offspring
Chromosome ga::CrossOver(Chromosome par1, Chromosome par2, vector<pm> PMList, vector<vm> VMList)
{
	// chromosome for offspring
	crossOverCount++;
	Chromosome child;
	if (VERBOSE)
		cout << "Starting CrossOver on Chromosomes " << par1.ChromosomeID << " and " << par2.ChromosomeID << endl;
	unsigned int cutIndex1 = 1 + static_cast<unsigned int>(rand()) % dimChromosome;
	unsigned int cutIndex2;
	if (cutIndex1 > 0)
		cutIndex2 = static_cast<unsigned int>(rand()) % cutIndex1;
	else
		cutIndex2 = 0;
	child.ChromosomeID = addedChromosomesIteration.size(); // pop.size();
	if (VERBOSE)
	{
		std::cout << "the Chromosome1 selected for crossOver reaction:";
		par1.printChromosomeStructure();
		std::cout << "the Chromosome2 selected for crossOver reaction:";
		par2.printChromosomeStructure();
	}
	// 1-fill the new Chromosome with X_BS(Chromosome with best stability level) up to index alphIdx2
	for (unsigned int i = 0; i <= cutIndex2; i++)
	{
		child.structure.push_back(par1.structure[i]);
	}
	// 2-fill the rest of new Chromosome (>alphIdx2) with the main Chromosome
	for (unsigned int i = cutIndex2 + 1; i < par2.structure.size(); i++)
	{
		child.structure.push_back(par2.structure[i]);
	}
	if (VERBOSE)
		child.printChromosomeStructure();
	// 3-remove repeated indices
	child.putNullValueOnRepeatingVMIndicesGa(cutIndex2); // the repeating indices (from the beginning to apphaIdx2)should be replaced with _ value (null or a specific value eg m where m is the number of VMs and no vm has index m)
	// 4-reinsert missing indices to form a valid structure
	if (VERBOSE)
		child.printChromosomeStructure();
	vector<vmGene> missingVMs = child.findMissingVMIndicesGa(par1);
	child.insertVMListRandomIntoBubllesGa(missingVMs, par1.structure.size());
	child.calculateFitChromosome(PMList, VMList);
	if (VERBOSE)
	{
		child.printChromosomeStructure();
	}
	child.ChromosomeID = addedChromosomesIteration.size(); // findValidChromosomeID();
	updateBestChromosome(child);
	// pop.push_back(New1);
	if (VERBOSE){
		cout << endl
				<< "Chromosome ID: " << child.ChromosomeID << endl;
	}
	addedChromosomesIteration.push_back(child);

	/*
	vector<vmGene> child_chromosome;
	vector<vmGene> temp1_chromosome;
	vector<vmGene> temp2_chromosome;
	 int len = par2.structure.size();
	 for(int i = 0;i<len;i++){
		temp1_chromosome.push_back(par2.structure[i]);
		temp2_chromosome.push_back(structure[i]);
	 }
	 int remaining=len;
	 for(int i = 0;i<len;i++)
	 {
		 // random probability
		 float p = randomNM(0, 100)/100;
		 int selected=rand()%remaining;
		 // if prob is less than 0.45, insert Chromosome
		 // from parent 1
		 if(p < 0.5){
			int selected_vm=temp1_chromosome[selected];
			 child_chromosome.push_back(selected_vm);
			 temp1_chromosome.erase(temp1_chromosome.begin()+selected);
			 temp2_chromosome=Chromosome_remove(temp2_chromosome,selected_vm);
			 remaining--;
		 }
		 // if prob is between 0.45 and 0.90, insert
		 // Chromosome from parent 2
		 else {//if(p < 0.90){
			int selected_vm=temp2_chromosome[selected];
			 child_chromosome.push_back(selected_vm);
			 temp1_chromosome=Chromosome_remove(temp1_chromosome,selected_vm);
			 temp2_chromosome.erase(temp2_chromosome.begin()+selected);
			 remaining--;
		 }
	 }
	 */
	return child;
};

///===========GA
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
bool operator<(const Chromosome &ind1, const Chromosome &ind2)
{
	return ind1.fit < ind2.fit;
}
float res_wastage_ffd_in_ga=0;
int num_active_pm_ffd_in_ga=0;

*/

/*

// Driver code
pair<double,double> main_GA(vector<pm> pm_set,vector<vm> vm_set)
{
	srand((unsigned)(time(0)));
	for(int i=0;i<num_vm_experiment;i++) Chromosomes[i]=i;
	ofstream ga_convergence;
	ga_convergence.open ("conv_GA.csv");
	ga_convergence << "iteration,fitness\n";
	// current generation
	int generation = 0;

	vector<Chromosome> population;
	bool found = false;
	vector<vm> VMlist2;
	ffd ffd1(pm_set,vm_set,  VMlist2);
	pow_ffd_in_ga=ffd1.pow_dis;
	// create initial population
	 cout<<endl<<"gnome:"<<endl;
	for(int i = 0;i<POPULATION_SIZE_GA;i++)
	{
		vector<int> gnome = create_gnome();
		//for(int j=0;j<num_vm_experiment;j++) cout<<" "<<gnome[j];
	   // cout<<endl;
		population.push_back(Chromosome(gnome,pm_set,vm_set));
	}
	int cnt=0;
	int cnt_print=0;
	while(cnt<limit_ga and FuncEval_ga<=max_FE_ga)
	{
		// sort the population in increasing order of fitness score
		sort(population.begin(), population.end());

		// if the Chromosome having lowest fitness score ie.
		// 0 then we know that we have reached to the target
		// and break the loop

		// Otherwise generate new offsprings for new generation
		vector<Chromosome> new_generation;

		// Perform Elitism, that mean 10% of fittest population
		// goes to the next generation
		int s = (10*POPULATION_SIZE_GA)/100;
		for(int i = 0;i<s;i++)
			new_generation.push_back(population[i]);

		// From 50% of fittest population, Chromosomes
		// will mate to produce offspring
		s = (90*POPULATION_SIZE_GA)/100;
		for(int i = 0;i<s;i++)
		{
		 //   int len = population.size();
			int r = randomNM(0, 50);
			Chromosome parent1 = population[r];
			r = randomNM(0, 50);
			Chromosome parent2 = population[r];
			Chromosome offspring = parent1.mate(parent2, pm_set,  vm_set);
			new_generation.push_back(offspring);
		}
		new_generation=mutate(new_generation);
		population = new_generation;
	 //   cout<< "Generation: " << generation << "\t";
	//    cout<< "Gnome: "<< endl;
	//    for(int i=0;i<num_vm_experiment;i++) cout<<population[0].chromosome[i] <<" ";
	//    cout<< endl<<"Fitness: "<< population[0].fitness << "\n";

		generation++;
		cnt++;
		cnt_print++;
		ga_convergence << generation<<","<<population[0].fit<<"\n";
		cout<<generation<<"...";
		if(cnt_print==100){
				cout <<endl;
				cnt_print=0;
			   }
	 }
	ga_convergence.close();
	pair<double,double> ret_pair;
	ret_pair=calculate_stat_ga( population[0],pm_set,vm_set);
	 cout<<endl<< "Generation: " << generation << "\t";
   // cout<< "String: "<< population[0].chromosome[0] <<"\t";
	cout<< "Fitness: "<< population[0].fit << "\n";
	cout<<endl<< "NUM of Functional Evaluations: " << FuncEval_ga;
	return ret_pair;
}
*/
