
#ifndef GA_H_
#define GA_H_

#include <iostream>
#include <vector>
#include <cmath>
#include <stdio.h>	/* printf, scanf, puts, NULL */
#include <stdlib.h> /* srand, rand */
#include <time.h>	/* time */
#include <iterator>
#include <vector>
#include <algorithm>
#include "defs.h"
#include "mapping.h"

#include <random>
#include <iomanip>
#include <string>
#include <map>
#include <bits/stdc++.h>
#include <fstream>

#include "Chromosome.h"
#include "ffd.h"

using namespace std;

// Valid Genes
// int Chromosomes[num_vm_experiment];

struct Individual
{
	unsigned int id;
	vector<int> chromosome;
	vm vmInfo; // to access its resource values
};

class ga
{
private:
public:
	ga();
	virtual ~ga();
	ga(int epoch, int pop_size, int num_dims);
	unsigned int epoch;
	unsigned int currentIteration;
	unsigned int pop_size;
	bool sort_flag;
	unsigned int dimChromosome;
	float bestFit;
	float worstFit;
	unsigned int numFuncEval_ga;
	unsigned int crossOverCount, mutationCount;
	const double EPSILON = 1e-8;
	std::vector<Chromosome> pop;
	std::vector<Chromosome> addedChromosomesIteration;
	std::vector<Chromosome> toBeRemovedChromosomesIteration;
	Chromosome bestChromosome;
	Chromosome worstChromosome;
	Chromosome bestEnrichmentBound; // X_SB
	Chromosome ChromosomeBestStabLevel;
	int mappingSol[num_vm_experiment];
	int numActive_pm;
	mapping Solution;
	float bestPow_dis;
	float bestResourceWastage;
	float bestStabLevel;
	float enrichmentBound;
	void evolve(vector<pm> PMList, vector<vm> VMList, std::ofstream &fileName, unsigned int iterationForAvg);
	void initialize(vector<vm> VMList);
	void calculateFitnessPop(vector<pm> PMList, vector<vm> VMList); // calculate Chromosomes' fitness and find best and worst Chromosomes and save them in bestChromosome and worstChromosome Chromosomes
	void sortPopFitnessDecreasing();								// calculate Chromosomes' fitness and find best and worst Chromosomes and save them in bestChromosome and worstChromosome Chromosomes

	vector<Chromosome> mutate(vector<Chromosome> theChromosome); //,vector<pm> PMList,vector<vm> VMList);
	Chromosome CrossOver(Chromosome parent1, Chromosome parent2, vector<pm> PMList, vector<vm> VMList);
	vector<Chromosome> sortPopByFitnessDecreasingly();
	unsigned int findChromosomeIdInPop(Chromosome theChromosome);
	void printPop();
	void printChromosomeVector(vector<Chromosome> vectChromosomes);
	void printFitPop();
	unsigned int findValidChromosomeID(); // search into pop and find free Chromosome IDs and return a valid one
	void updateBestChromosome(Chromosome theNewChromosome);
	void updateWorstChromosome(Chromosome theNewChromosome);
	void modifyChromosomeIndices(); // to correct Chromosomes IDs and give them IDs from 0 up
	void setBestAndWorstChromosomes();
	void positronReactionOld(Chromosome theChromosome, vector<pm> PMList, vector<vm> VMList);
};
#endif /* GA_H_ */
