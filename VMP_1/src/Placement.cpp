/*
 * Placement.cpp
 *
 *  Created on: Jan 15, 2024
 *      Author: mahdi
 */


#include "Placement.h"
#include "ffd.h"

Placement::Placement()
{
}

Placement::~Placement()
{
}
Placement::Placement(unsigned int ID)
{
	PlacementID = ID;
}

vector<vm> Placement::createVMListPlacement(vector<vm> VMList)
{
	vector<vm> VMListPlacement;
	for (auto x : structure)
	{
		vm theVM;
		theVM = VMList[x.id];
		VMListPlacement.push_back(theVM);
	}
	return VMListPlacement;
}

float fitnessFunctionHho(float power, float wastage)
{
	float alpha = 1.0;
	float beta = 10.0;
	float fitness = alpha * power + beta * wastage;
	// if maximizing: fitness=1/fitness;
	return fitness;
}

void Placement::calculateFitPlacement(vector<pm> PMList, vector<vm> VMList)
{
	vector<vm> VMListPlacement = this->createVMListPlacement(VMList);
	ofstream fakeFilePointer;
	ffd ffdPlacement(PMList, VMListPlacement, 0.0, 0.0, 0.0, fakeFilePointer, 0, 0);
	float fitness = fitnessFunctionHho(ffdPlacement.Solution.power, ffdPlacement.Solution.wastage);
	this->fit = fitness;
}

float Placement::calculatePowerPlacement(vector<pm> PMList, vector<vm> VMList)
{
	vector<vm> VMListPlacement = this->createVMListPlacement(VMList);
	ofstream fakeFilePointer;
	ffd ffdPlacement(PMList, VMListPlacement, 0.0, 0.0, 0.0, fakeFilePointer, 0, 0);
	float fitness = fitnessFunctionHho(ffdPlacement.Solution.power, ffdPlacement.Solution.wastage);
	return ffdPlacement.Solution.power;
}

mapping Placement::fitPlacementSequenceUsingFF(vector<pm> PMList, vector<vm> VMList)
{
	vector<vm> VMListPlacement = this->createVMListPlacement(VMList);
	ofstream fakeFilePointer;
	ffd ffdPlacement(PMList, VMListPlacement, 0.0, 0.0, 0.0, fakeFilePointer, 0, 0);
	float fitness = fitnessFunctionHho(ffdPlacement.Solution.power, ffdPlacement.Solution.wastage);

	return ffdPlacement.Solution;
}

vector<vmPlacement> Placement::findMissingVMIndicesHho(Placement thePlacementWithAllVMs)
{
	// return a list of vmPlacement containing those missing VM indices (to be inserted into the structure to for a valid one)
	vector<unsigned int> allVMIndices(structure.size());
	iota(allVMIndices.begin(), allVMIndices.end(), 0);
	for (auto x : allVMIndices)
		cout << " " << x;
	cout << endl;
	vector<vmPlacement> missingIndices;
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
			vmPlacement theVMPlacement;
			theVMPlacement.id = x;
			missingIndices.push_back(theVMPlacement);
			// cout<<" missing Idx: "<<theVMPlacement.id<<endl;
		}
	}
	return missingIndices;
}

void Placement::insertVMListRandomIntoBubllesHho(vector<vmPlacement> missingVMs, unsigned int m)
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
	checkValidityPlacement();
}

void Placement::insertVMListRandomHho(vector<vmPlacement> missingVMs)
{
	// insert missing VMs into random places in the Placement (no bubble exists in the main Placement)
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
	checkValidityPlacement();
}

void Placement::insertVMListRandomEndHho(vector<vmPlacement> missingVMs)
{
	// insert missing VMs into random places starting from the end of the Placement (the VMs are selected from missingVMs at random)
	unsigned int totalVMindices = missingVMs.size();
	unsigned int remainingLenghtList = totalVMindices;
	for (unsigned int i = 0; i < totalVMindices; i++)
	{
		unsigned int randomIdxmissing = rand() % remainingLenghtList;
		structure.insert(structure.end(), missingVMs.at(randomIdxmissing)); //[idxMissing]=(missingVMs.at(randomIdx));
		remainingLenghtList--;
		missingVMs.erase(missingVMs.begin() + randomIdxmissing);
	}
	checkValidityPlacement();
}

bool vmExistsHho(vector<vmPlacement> structPlacement, vmPlacement VMIdx)
{
	for (auto x : structPlacement)
	{
		if (x.id == VMIdx.id)
		{
			return true;
		}
	}
	return false;
}

void Placement::putNullValueOnRepeatingVMIndicesHho(unsigned int alphaIdx2)
{
	// for instance if alphIdx=2 and Placement structure={2,4,5,1,5,0,3,4} (note that now the Placement does not have a valid structure)
	// then, the output is: {2,4,5,1,m,0,3,m}
	unsigned m = structure.size();
	vector<vmPlacement> structureofBestStabPlacement;
	for (unsigned int i = 0; i <= alphaIdx2; i++)
		structureofBestStabPlacement.push_back(structure[i]);
	for (unsigned int i = alphaIdx2 + 1; i < m; i++)
	{
		if (vmExistsHho(structureofBestStabPlacement, structure[i]))
		{
			structure[i].id = m;
		}
	}
}

void Placement::checkValidityPlacement()
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
					this->printPlacementStructure();
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

void Placement::printPlacementStructure()
{
	cout << "Placement structure: ";
	for (auto y : structure)
	{
		cout << " " << y.id;
	}
	cout << endl;
}

void Placement::putBubbleInIndicesWithDifferentCPUHho(Placement Placement1, Placement Placement2)
{
	// if for index i CPU of Placement1,2 has a difference of <=10%, hold the index (from Placement1), otherwise, fill it with m (as a shallow or bubble)
	Placement newPlacement;
	newPlacement = Placement1;
	for (unsigned int i = 0; i < Placement1.structure.size(); i++)
	{
		float cpu1 = structure[i].vmInfo.res.cpu;
		float cpu2 = Placement2.structure[i].vmInfo.res.cpu;
		float percentDiff = 100 * abs(cpu1 - cpu2) / cpu1;
		if (percentDiff > 10.0)
		{
			vm nullVMInfo;
			nullVMInfo.fillNullValues();
			structure[i].id = Placement1.structure.size();
			structure[i].vmInfo = nullVMInfo;
		}
	}
}

vector<vmPlacement> Placement::shiftRightPlacementStructureHho(unsigned int numShift, Placement thePlacement)
{
	// structure is empty. first, put structure of thePlacement into structure (of the new Placement) then apply the shift and return removed VMs
	vmPlacement lastPos;
	structure = thePlacement.structure;
	vector<vmPlacement> removedList;
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

void Placement::rotateRightPlacementStructureHho(unsigned int numRotate, Placement thePlacement)
{
	// structure is empty. first, put structure of thePlacement into structure (of the new Placement) then apply the rotation
	// since no VM is removed, no return is required
	vmPlacement lastPos;
	structure = thePlacement.structure;
	for (unsigned int i = 0; i < numRotate; ++i)
	{
		unsigned int idx = structure.size() - 1;
		lastPos = structure[idx];
		structure.erase(structure.begin() + idx);
		structure.insert(structure.begin(), lastPos);
	}
}
