/*
 * BF.cpp
 *
 *  Created on: Oct 19, 2023
 *      Author: mkiani
 */

#include <bits/stdc++.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <stdlib.h> /* abs */
#include "BF.h"
// #include "pm.h"
#include "idata.h"

using namespace std;

void BF::initialize_free_cap_pmsBF(vector<pm> pm_list)
{
	for (int i = 0; i < num_pm_experiment; i++)
	{
		free_cap_pms[i] = pm_list[i].capacity.cpu;
		free_cap_mem_pms[i] = pm_list[i].capacity.mem;
		free_cap_net_pms[i] = pm_list[i].capacity.bw;
	}
}

void BF::update_free_cap_pmsBF(int cap_cpu, int cap_mem, int cap_net, int pm, int backward_modification_traffic)
{
	free_cap_net_pms[pm] += backward_modification_traffic;
	free_cap_pms[pm] -= cap_cpu;
	free_cap_mem_pms[pm] -= cap_mem;
	free_cap_net_pms[pm] -= cap_net;
}

int BF::num_active_pmsBF(vector<vm> vm_list)
{
	int act_pms = 0;
	for (unsigned int i = 0; i < vm_list.size(); i++)
	{
		for (unsigned int j = 0; j < vm_list.size(); j++)
		{
			if (solution[j] == i)
			{
				act_pms++;
				break;
			}
		}
		// if(solution[i]<num_vm_experiment) act_pms++;
	}
	return act_pms;
}

BF::BF()
{
}

BF::~BF()
{
}

// parameter bool originalBF used to check if the BF is invoked by main (thus stast to be written to file) or by other meta-heuristics to calcualte fitness
BF::BF(vector<pm> pm_list_m, vector<vm> vm_list_m, float RpVal, float RmVal, float PVal, std::ofstream &fileName, bool originalBF, unsigned int iterationForAvg)
{						//,int vm_to_vm_comm[][NUM_SERVICES*MAX_VM_PER_SERVICE]){
	vector<pm> pm_list; //=pm_list_m;
	vector<vm> vm_list; //=vm_list_m;
	Solution.iterForAvg = iterationForAvg;
	for (unsigned int i = 0; i < vm_list_m.size(); i++)
	{
		mappingSol[i] = pm_list_m.size();
		this->Solution.VPP.push_back(pm_list_m.size());
	}
	long int total_cpu_req = 0;
	for (unsigned int ij = 0; ij < vm_list_m.size(); ij++)
		total_cpu_req += vm_list_m[ij].res.cpu;
	cout << endl
		 << "total CPU requests of all VMs (X): " << total_cpu_req << endl;
	for (vector<pm>::iterator it = pm_list_m.begin(); it != pm_list_m.end(); ++it)
	{
		pm_list.push_back(*it);
	}
	/*for(auto x:pm_list){
		x.state=0;
		cout<<"PM-"<<x.id<<" state: "<<x.state<<endl;
	}*/

	for (vector<vm>::iterator it = vm_list_m.begin(); it != vm_list_m.end(); ++it)
	{
		vm_list.push_back(*it);
	}
	for (unsigned int i = 0; i < vm_list_m.size(); i++)
	{
		solution[i] = vm_list_m.size();
		original_vm_indexes[i] = vm_list[i].id;
	}
	// shuffle if called by the main function not by other meta-heurs that use BF to calculate fitness (thus for their own VM order)
	unsigned int bestPMID;
	initialize_free_cap_pmsBF(pm_list);
	int vmidx = 0;
	int backward_modification_traffic = 0;
	for (unsigned int j = 0; j < vm_list_m.size(); j++) // vector<vm>::iterator it = vm_list.begin(); it != vm_list.end(); ++it)
	{
		// find the PM that can accommodate the next VM in the list
		bestPMID = findBestPM(vm_list[j], pm_list);
		pm_list[bestPMID].state = 1;
		unsigned int voi = vm_list[j].id;
		unsigned int rcpu = vm_list[j].res.cpu; // it->requested_cpu;
		unsigned int rmem = vm_list[j].res.mem;
		unsigned int rnet = 0;

		solution[vm_list[j].id] = bestPMID;
		mappingSol[voi] = bestPMID;
		this->Solution.VPP.push_back(bestPMID);
		update_free_cap_pmsBF(rcpu, rmem, rnet, bestPMID, backward_modification_traffic);

		vmidx++;
	}
	double total_cpu_req1 = 0;
	double total_mem_req1 = 0;
	double total_net_req1 = 0;
	for (unsigned int ij = 0; ij < vm_list_m.size(); ij++)
	{
		total_cpu_req1 += vm_list[ij].res.cpu;
		total_mem_req1 += vm_list[ij].res.mem;
		total_net_req1 += vm_list[ij].res.bw;
	}
	cout << "total CPU requests of all VMs BF): " << total_cpu_req1;
	cout << endl
		 << "total Mem requests of all VMs(BF): " << total_mem_req1;
	cout << endl
		 << "total BW requests of all VMs(BF): " << total_net_req1;
	int num_act_pms = num_active_pmsBF(vm_list_m);
	cout << endl;
	cout << "Active PM count (BF): " << num_act_pms;

	if (VERBOSE)
	{
		cout << endl
			 << "Free capacity of the PMs(BF):" << endl;
		for (int i = 0; i < num_pm_experiment; i++)
		{
			cout << free_cap_pms[i] << " ";
		}
		cout << endl;
	}
	for (int i = 0; i < num_pm_experiment; i++)
	{
		this->Solution.freeCPU[i] = free_cap_pms[i];
		this->Solution.freeMEM[i] = free_cap_mem_pms[i];
	}

	Solution.PValue = PVal;
	Solution.RmValue = RmVal;
	Solution.RpValue = RpVal;
	Solution.numActivePMs = num_act_pms;
	Solution.numVMs = vm_list_m.size();
	Solution.calculateMappingPower(pm_list);
	Solution.calculateMappingWastageAndBalance(pm_list);
	if (originalBF)
		Solution.printMappingStatsIntoFile(fileName, pm_list_m);
}

unsigned int BF::findBestPM(vm theVM, vector<pm> PMList)
{
	// randomize the VMs
	unsigned int bestPMIndex = PMList.size();
	unsigned int rcpu = theVM.res.cpu; // it->requested_cpu;
	unsigned int rmem = theVM.res.mem;
	unsigned int minFree = 1e8;
	for (unsigned int i = 0; i < PMList.size(); i++)
	{
		int signedDiff = free_cap_pms[i] - rcpu;
		unsigned int diffCPU = abs(signedDiff);
		if (free_cap_pms[i] > rcpu and free_cap_mem_pms[i] > rmem and diffCPU < minFree and (free_cap_pms[i] < PMList[i].capacity.cpu and free_cap_mem_pms[i] < PMList[i].capacity.mem))
		{
			minFree = diffCPU; // free_cap_pms[i];
			bestPMIndex = i;
		}
	}
	if (bestPMIndex == PMList.size())
	{
		unsigned int i = 0;
		while (free_cap_pms[i] < PMList[i].capacity.cpu and free_cap_mem_pms[i] < PMList[i].capacity.mem)
			i++;
		bestPMIndex = i; // no active PM capable of taking the VM found, hence activate the first inactive PM (all PMs capable of taking any of the VMs thus the first inactive PM can take the VM). Although, this may contradict BF and one can find the first BEST capable PM. One can first sort PMs increasingly
	}
	return bestPMIndex;
}

void BF::analyzeMapping()
{
}
