/*
 * mapping.h
 *
 *  Created on: Oct 3, 2023
 *      Author: mkiani
 */

#ifndef MAPPING_H_
#define MAPPING_H_

#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <vector>

#include "defs.h"
#include "pm.h"

using namespace std;

struct twoFloats
{
	float one; // level 1 (inter-pod)
	float two;
};

class mapping
{
public:
	mapping();
	virtual ~mapping();
	vector<int> VPP;	 // VM to PM placement solution: a VM-length array, the value at each index showing the assigned PM to the VM
	unsigned int numPMs; // the number of used PMs in the mapping
	unsigned int numVMs;
	unsigned int numActivePMs;
	float PValue;
	float RpValue;
	float RmValue;
	float power;   // power consumption of the mapping solution
	float wastage; // resource waste of the mapping solution
	float loadBalanceCPU;
	float loadBalanceMEM;
	float utilCPU;
	float utilMEM;
	unsigned int iterForAvg;
	unsigned int totaProcessedVMs;
	vector<float> analyzeMapping(bool set_result);
	unsigned int freeCPU[num_pm_experiment];
	unsigned int freeMEM[num_pm_experiment];
	unsigned int freeBW[num_pm_experiment];
	void prinMappingScores();
	unsigned int countActivePMs();
	void calculateMappingPower(vector<pm> pm_list);
	void calculateMappingEnergy();
	void calculateMappingWastageAndBalance(vector<pm> pm_list);
	void printMappingStatsIntoFile(std::ofstream &fileName, vector<pm> PMList);

protected:
private:
};

#endif /* MAPPING_H_ */
