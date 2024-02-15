/*
 * Chromosome.cpp
 *
 *  Created on: Nov 1, 2023
 *      Author: mkiani
 */

#include "Chromosome.h"
#include "ffd.h"

Chromosome::Chromosome()
{
}

Chromosome::~Chromosome()
{
}
Chromosome::Chromosome(unsigned int ID)
{
	ChromosomeID = ID;
}

vector<vm> Chromosome::createVMListChromosome(vector<vm> VMList)
{
	vector<vm> VMListChromosome;
	for (auto x : structure)
	{
		vm theVM;
		theVM = VMList[x.id];
		VMListChromosome.push_back(theVM);
	}
	return VMListChromosome;
}

float fitnessFunctionGa(float power, float wastage)
{
	float alpha = 1.0;
	float beta = 10.0;
	float fitness = alpha * power + beta * wastage;
	// if maximizing: fitness=1/fitness;
	return fitness;
}

void Chromosome::calculateFitChromosome(vector<pm> PMList, vector<vm> VMList)
{
	vector<vm> VMListChromosome = this->createVMListChromosome(VMList);
	ofstream fakeFilePointer;
	ffd ffdChromosome(PMList, VMListChromosome, 0.0, 0.0, 0.0, fakeFilePointer, 0, 0);
	float fitness = fitnessFunctionGa(ffdChromosome.Solution.power, ffdChromosome.Solution.wastage);
	this->fit = fitness;
}

float Chromosome::calculatePowerChromosome(vector<pm> PMList, vector<vm> VMList)
{
	vector<vm> VMListChromosome = this->createVMListChromosome(VMList);
	ofstream fakeFilePointer;
	ffd ffdChromosome(PMList, VMListChromosome, 0.0, 0.0, 0.0, fakeFilePointer, 0, 0);
	float fitness = fitnessFunctionGa(ffdChromosome.Solution.power, ffdChromosome.Solution.wastage);
	return ffdChromosome.Solution.power;
}

mapping Chromosome::fitChromosomeSequenceUsingFF(vector<pm> PMList, vector<vm> VMList)
{
	vector<vm> VMListChromosome = this->createVMListChromosome(VMList);
	ofstream fakeFilePointer;
	ffd ffdChromosome(PMList, VMListChromosome, 0.0, 0.0, 0.0, fakeFilePointer, 0, 0);
	float fitness = fitnessFunctionGa(ffdChromosome.Solution.power, ffdChromosome.Solution.wastage);

	return ffdChromosome.Solution;
}

vector<vmGene> Chromosome::findMissingVMIndicesGa(Chromosome theChromosomeWithAllVMs)
{
	// return a list of vmChromosome containing those missing VM indices (to be inserted into the structure to for a valid one)
	vector<unsigned int> allVMIndices(structure.size());
	iota(allVMIndices.begin(), allVMIndices.end(), 0);
	for (auto x : allVMIndices)
		cout << " " << x;
	cout << endl;
	vector<vmGene> missingIndices;
	for (auto x : allVMIndices)
	{
		bool exists = false;
		for (auto y : structure)
		{
			if (y.id == x)
			{
				exists = true;
				break;
			}
		}
		if (!exists)
		{
			vmGene theVMChromosome;
			theVMChromosome.id = x;
			missingIndices.push_back(theVMChromosome);
			// cout<<" missing Idx: "<<theVMChromosome.id<<endl;
		}
	}
	return missingIndices;
}

void Chromosome::insertVMListRandomIntoBubllesGa(vector<vmGene> missingVMs, unsigned int m)
{
	unsigned int totalVMindices = missingVMs.size();
	unsigned int remainingLenghtList = totalVMindices;
	unsigned int idxMissing = 0;
	for (unsigned int i = 0; i < totalVMindices; i++)
	{
		// note those missing places in the structures are set to m
		// traverse the structure to reach to locations containing m and then fill them with a random sequence of missing VMs
		while (structure[idxMissing].id != m)
		{
			idxMissing++;
		}
		unsigned int randomIdx = rand() % remainingLenghtList;
		structure.erase(structure.begin() + idxMissing);
		structure.insert(structure.begin() + idxMissing, missingVMs.at(randomIdx)); //[idxMissing]=(missingVMs.at(randomIdx));
		remainingLenghtList--;
		missingVMs.erase(missingVMs.begin() + randomIdx);
	}
	checkValidityChromosome();
}

void Chromosome::insertVMListRandomGa(vector<vmGene> missingVMs)
{
	// insert missing VMs into random places in the Chromosome (no bubble exists in the main Chromosome)
	unsigned int totalVMindices = missingVMs.size();
	unsigned int remainingLenghtList = totalVMindices;
	for (unsigned int i = 0; i < totalVMindices; i++)
	{
		unsigned int randomIdxmissing = rand() % remainingLenghtList;
		unsigned int randomIdxStructure = rand() % structure.size();
		structure.insert(structure.begin() + randomIdxStructure, missingVMs.at(randomIdxmissing)); //[idxMissing]=(missingVMs.at(randomIdx));
		remainingLenghtList--;
		missingVMs.erase(missingVMs.begin() + randomIdxmissing);
	}
	checkValidityChromosome();
}

void Chromosome::insertVMListRandomEndGa(vector<vmGene> missingVMs)
{
	// insert missing VMs into random places starting from the end of the Chromosome (the VMs are selected from missingVMs at random)
	unsigned int totalVMindices = missingVMs.size();
	unsigned int remainingLenghtList = totalVMindices;
	for (unsigned int i = 0; i < totalVMindices; i++)
	{
		unsigned int randomIdxmissing = rand() % remainingLenghtList;
		structure.insert(structure.end(), missingVMs.at(randomIdxmissing)); //[idxMissing]=(missingVMs.at(randomIdx));
		remainingLenghtList--;
		missingVMs.erase(missingVMs.begin() + randomIdxmissing);
	}
	checkValidityChromosome();
}

bool vmExistsGA(vector<vmGene> structChromosome, vmGene VMIdx)
{
	for (auto x : structChromosome)
	{
		if (x.id == VMIdx.id)
		{
			return true;
		}
	}
	return false;
}

void Chromosome::putNullValueOnRepeatingVMIndicesGa(unsigned int alphaIdx2)
{
	// for instance if alphIdx=2 and Chromosome structure={2,4,5,1,5,0,3,4} (note that now the Chromosome does not have a valid structure)
	// then, the output is: {2,4,5,1,m,0,3,m}
	unsigned m = structure.size();
	vector<vmGene> structureofBestStabChromosome;
	for (unsigned int i = 0; i <= alphaIdx2; i++)
		structureofBestStabChromosome.push_back(structure[i]);
	for (unsigned int i = alphaIdx2 + 1; i < m; i++)
	{
		if (vmExistsGA(structureofBestStabChromosome, structure[i]))
		{
			structure[i].id = m;
		}
	}
}

void Chromosome::checkValidityChromosome()
{
	// TODO
	// no repeated index is acceptable
	// no missing index is acceptable
	vector<unsigned int> vmListIndices(structure.size());
	iota(vmListIndices.begin(), vmListIndices.end(), 0);
	for (auto x : vmListIndices)
	{
		bool seenBefore = false;
		for (auto y : structure)
		{
			if (y.id == x)
			{
				if (!seenBefore)
				{
					seenBefore = true;
				}
				else
				{
					cout << "repeated index: " << y.id << endl;
					this->printChromosomeStructure();
				}
			}
		}
		if (!seenBefore)
		{
			// missing index
			cout << "missing index: " << x << endl;
		}
	}
}

void Chromosome::printChromosomeStructure()
{
	cout << "Chromosome structure: ";
	for (auto y : structure)
	{
		cout << " " << y.id;
	}
	cout << endl;
}

void Chromosome::putBubbleInIndicesWithDifferentCPUGa(Chromosome Chromosome1, Chromosome Chromosome2)
{
	// if for index i CPU of Chromosome1,2 has a difference of <=10%, hold the index (from Chromosome1), otherwise, fill it with m (as a shallow or bubble)
	Chromosome newChromosome;
	newChromosome = Chromosome1;
	for (unsigned int i = 0; i < Chromosome1.structure.size(); i++)
	{
		float cpu1 = structure[i].vmInfo.res.cpu;
		float cpu2 = Chromosome2.structure[i].vmInfo.res.cpu;
		float percentDiff = 100 * abs(cpu1 - cpu2) / cpu1;
		if (percentDiff > 10.0)
		{
			vm nullVMInfo;
			nullVMInfo.fillNullValues();
			structure[i].id = Chromosome1.structure.size();
			structure[i].vmInfo = nullVMInfo;
		}
	}
}

vector<vmGene> Chromosome::shiftRightChromosomeStructureGa(unsigned int numShift, Chromosome theChromosome)
{
	// structure is empty. first, put structure of theChromosome into structure (of the new Chromosome) then apply the shift and return removed VMs
	vmGene lastPos;
	structure = theChromosome.structure;
	vector<vmGene> removedList;
	for (unsigned int i = 0; i < numShift; ++i)
	{
		int idx = structure.size() - 1;
		if (idx < 1)
			cout << "Error the structure size is zero!!!" << endl;
		lastPos = structure[idx];
		removedList.push_back(lastPos);
		structure.erase(structure.begin() + idx);
	}
	return removedList;
}

void Chromosome::rotateRightChromosomeStructureGa(unsigned int numRotate, Chromosome theChromosome)
{
	// structure is empty. first, put structure of theChromosome into structure (of the new Chromosome) then apply the rotation
	// since no VM is removed, no return is required
	vmGene lastPos;
	structure = theChromosome.structure;
	for (unsigned int i = 0; i < numRotate; ++i)
	{
		unsigned int idx = structure.size() - 1;
		lastPos = structure[idx];
		structure.erase(structure.begin() + idx);
		structure.insert(structure.begin(), lastPos);
	}
}
