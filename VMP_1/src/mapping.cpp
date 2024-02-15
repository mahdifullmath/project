/*
 * mapping.cpp
 *
 *  Created on: Oct 3, 2023
 *      Author: mkiani
 */
#include "mapping.h"

mapping::mapping()
{
}

mapping::~mapping()
{
}

vector<float> mapping::analyzeMapping(bool set_result)
{
}

void mapping::prinMappingScores()
{
}

unsigned int mapping::countActivePMs()
{
}

void mapping::calculateMappingPower(vector<pm> pm_list)
{
	unsigned int cpu_overload_pm[num_pm_experiment];
	/*unsigned int mem_overload_pm[num_pm_experiment];
	unsigned int net_overload_pm[num_pm_experiment];*/
	for (unsigned int i = 0; i < num_pm_experiment; i++)
	{
		cpu_overload_pm[i] = 0;
		/*mem_overload_pm[i]=0;
		net_overload_pm[i]=0;*/
	}
	for (unsigned int i = 0; i < num_pm_experiment; i++)
	{
		cpu_overload_pm[i] = pm_list[i].capacity.cpu - freeCPU[i];
		/*mem_overload_pm[i]=pm_list[i].capacity.mem-freeMEM[i];
		net_overload_pm[i]=pm_list[i].capacity.bw-freeBW[i];*/
	}
	float server_pow = 0;
	for (unsigned int i = 0; i < num_pm_experiment; i++)
	{
		if (cpu_overload_pm[i] != 0)
			server_pow += 1.0 * pm_list[i].p_idle + 1.0 * (pm_list[i].p_max - pm_list[i].p_idle) * (cpu_overload_pm[i] / (1.0 * pm_list[i].capacity.cpu));
	}
	this->power = server_pow;
}

void mapping::calculateMappingEnergy()
{
}

void mapping::calculateMappingWastageAndBalance(vector<pm> pm_list)
{
	float res_wastage = 0;
	unsigned int cpu_overload_pm[num_pm_experiment];
	unsigned int mem_overload_pm[num_pm_experiment];
	// unsigned int net_overload_pm[num_pm_experiment];
	for (unsigned int i = 0; i < num_pm_experiment; i++)
	{
		cpu_overload_pm[i] = 0;
		mem_overload_pm[i] = 0;
		// net_overload_pm[i]=0;
	}
	for (unsigned int i = 0; i < num_pm_experiment; i++)
	{
		cpu_overload_pm[i] = pm_list[i].capacity.cpu - freeCPU[i];
		mem_overload_pm[i] = pm_list[i].capacity.mem - freeMEM[i];
		// net_overload_pm[i]=pm_list[i].capacity.bw-freeBW[i];
	}
	float avgCPUUtil = 0;
	float avgMEMUtil = 0;
	unsigned int activePMs = 0;
	for (unsigned int i = 0; i < num_pm_experiment; i++)
	{ // for each PM, calculate the wasted resource and sum up for all PMs
		if (cpu_overload_pm[i] > 0 and mem_overload_pm[i] > 0)
		{
			float L_cpu = (1.0 * freeCPU[i]) / (1.0 * pm_list[i].capacity.cpu);
			float L_mem = (1.0 * freeMEM[i]) / (1.0 * pm_list[i].capacity.mem);
			float numerator = abs(L_cpu - L_mem) + epsilon_wastage;
			float denumerator = cpu_overload_pm[i] + mem_overload_pm[i];
			res_wastage += numerator / denumerator;
			avgCPUUtil += cpu_overload_pm[i] / (1.0 * pm_list[i].capacity.cpu);
			;
			avgMEMUtil += mem_overload_pm[i] / (1.0 * pm_list[i].capacity.mem);
			;
			activePMs++;
		}
	}
	this->numActivePMs = activePMs;
	this->wastage = res_wastage;
	avgCPUUtil = avgCPUUtil / (1.0 * activePMs);
	avgMEMUtil = avgMEMUtil / (1.0 * activePMs);
	float accCPUUtilDiff = 0;
	float accMEMUtilDiff = 0;
	float accutilCPU = 0, accutilMEM = 0;
	for (unsigned int i = 0; i < num_pm_experiment; i++)
	{
		if (cpu_overload_pm[i] > 0 and mem_overload_pm[i] > 0)
		{
			float utilCPUPM = cpu_overload_pm[i] / pm_list[i].capacity.cpu;
			float utilMEMPM = mem_overload_pm[i] / pm_list[i].capacity.mem;
			accutilCPU += utilCPUPM;
			accutilMEM += utilMEMPM;
			accCPUUtilDiff += abs(utilCPUPM - avgCPUUtil);
			accMEMUtilDiff += abs(utilMEMPM - avgMEMUtil);
		}
	}
	this->loadBalanceCPU = accCPUUtilDiff / (1.0 * activePMs);
	this->loadBalanceMEM = accMEMUtilDiff / (1.0 * activePMs);
	this->utilCPU = accutilCPU / (1.0 * activePMs);
	this->utilMEM = accutilMEM / (1.0 * activePMs);
	if (VERBOSE)
	{
		cout << endl
			 << "CPU Load imBalance: " << accCPUUtilDiff / (1.0 * activePMs) << endl;
		cout << "MEM Load imBalance: " << accMEMUtilDiff / (1.0 * activePMs) << endl;
	}
}

void mapping::printMappingStatsIntoFile(std::ofstream &fileName, vector<pm> PMList)
{
	// PM resources:

	fileName << endl
			 << "||=====Placement analysis result Iteration " << iterForAvg << "=====||" << endl;
	fileName << endl
			 << "||=====PM utilization report=====||" << endl;

	float accUtilCPU = 0;
	float accUtilMEM = 0;
	for (unsigned int i = 0; i < numActivePMs; i++)
	{
		fileName << "PM-" << i << " util CPU/Mem: " << 100.0 * (PMList[i].capacity.cpu - freeCPU[i]) / PMList[i].capacity.cpu << " / " << 100.0 * (PMList[i].capacity.mem - freeMEM[i]) / PMList[i].capacity.mem << endl;
		accUtilCPU += 100.0 * (PMList[i].capacity.cpu - freeCPU[i]) / PMList[i].capacity.cpu;
		accUtilMEM += 100.0 * (PMList[i].capacity.mem - freeMEM[i]) / PMList[i].capacity.mem;
	}
	fileName << endl;
	fileName << "avg CPU util: " << accUtilCPU / numActivePMs << endl;
	fileName << "avg MEM util: " << accUtilMEM / numActivePMs << endl;
	fileName << endl;

	fileName << endl
			 << "||====Main parameters====||" << endl;
	fileName << "Num active PMs: " << numActivePMs << endl;
	fileName << "Num VMs: " << numVMs << endl;
	fileName << "Settings (P value |  Rp  | Rm): " << PValue << " | " << RpValue << " | " << RmValue << endl;
	fileName << "Power: " << power << " Watts" << endl;
	fileName << "Wastage: " << wastage << endl;
	fileName << "CPU Load Balance: " << loadBalanceCPU << endl;
	fileName << "MEM Load Balance: " << loadBalanceMEM << endl;
}
