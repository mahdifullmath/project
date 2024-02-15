/*
 * ffd.cpp
 *
 *  Created on: Oct 2, 2023
 *      Author: mkiani
 */

#include <bits/stdc++.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include "ffd.h"
// #include "pm.h"
#include "idata.h"

using namespace std;

enum ffd_type_selector
{
	FFD_CPU,
	FFD_MEM,
	FFD_NET
};
ffd_type_selector ffd_type = FFD_CPU;

unsigned int solution[NUM_SERVICES * MAX_VM_PER_SERVICE]; // each element shows which PM hosts the VM (index=VM, value=PM)
unsigned int free_cap_pms[num_pm_experiment];
unsigned int free_cap_mem_pms[num_pm_experiment];
unsigned int free_cap_net_pms[num_pm_experiment];
unsigned int original_vm_indexes[NUM_SERVICES * MAX_VM_PER_SERVICE];

void initialize_free_cap_pms(vector<pm> pm_list)
{
	for (int i = 0; i < num_pm_experiment; i++)
	{
		free_cap_pms[i] = pm_list[i].capacity.cpu;
		free_cap_mem_pms[i] = pm_list[i].capacity.mem;
		free_cap_net_pms[i] = pm_list[i].capacity.bw;
	}
}

void update_free_cap_pms(int cap_cpu, int cap_mem, int cap_net, int pm, int backward_modification_traffic)
{
	free_cap_net_pms[pm] += backward_modification_traffic;
	free_cap_pms[pm] -= cap_cpu;
	free_cap_mem_pms[pm] -= cap_mem;
	free_cap_net_pms[pm] -= cap_net;
}

int num_active_pms(vector<vm> vm_list)
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

void calculate_balance()
{
}

ffd::ffd()
{
}
ffd::ffd(vector<pm> pm_list, vector<vm> vm_list, int x)
{
}

ffd::~ffd()
{
}

// parameter bool originalFFD used to check if the FFD is invoked by main (thus stast to be written to file) or by other meta-heuristics to calcualte fitness
ffd::ffd(vector<pm> pm_list_m, vector<vm> vm_list_m, float RpVal, float RmVal, float PVal, std::ofstream &fileName, bool originalFFD, unsigned int iterationForAvg)
{						//,int vm_to_vm_comm[][NUM_SERVICES*MAX_VM_PER_SERVICE]){
	vector<pm> pm_list; //=pm_list_m;
	vector<vm> vm_list; //=vm_list_m;
	Solution.iterForAvg = iterationForAvg;
	// int vm_pm_mapping[vm_list_m.size()];	//element i containing x shows VMi is mapped to PMx
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
	for (vector<vm>::iterator it = vm_list_m.begin(); it != vm_list_m.end(); ++it)
	{
		vm_list.push_back(*it);
	}
	for (unsigned int i = 0; i < vm_list_m.size(); i++)
	{
		solution[i] = vm_list_m.size();
		original_vm_indexes[i] = vm_list[i].id;
	}
	// sort only if called by the main function not by other meta-heurs that use FFD to calculate fitness (thus for their own VM order)
	if (originalFFD)
		vm_list = sort_vms_decreasing_cpu(vm_list);
	initialize_free_cap_pms(pm_list);
	int vmidx = 0;
	int backward_modification_traffic = 0;
	for (unsigned int j = 0; j < vm_list_m.size(); j++) // vector<vm>::iterator it = vm_list.begin(); it != vm_list.end(); ++it)
	{
		// find the PM that can accommodate the next VM in the list
		unsigned int i = 0;
		unsigned int voi = vm_list[j].id;
		unsigned int rcpu = vm_list[j].res.cpu; // it->requested_cpu;
		unsigned int rmem = vm_list[j].res.mem;
		unsigned int rnet; //=vm_list[j].requested_net;
		unsigned int rnet_total_vm = 0;

		rnet = rnet_total_vm;
		// int comm_vm_idx=0;
		backward_modification_traffic = 0;

		while (free_cap_pms[i] < rcpu or free_cap_mem_pms[i] < rmem or (free_cap_net_pms[i] + backward_modification_traffic) < rnet)
		{ // !net_requirement_pass(vm_list[j],i,pm_list)){
			i++;
			rnet = rnet_total_vm;
			// comm_vm_idx=0;
			backward_modification_traffic = 0;
		}
		solution[vm_list[j].id] = i;
		mappingSol[voi] = i;
		this->Solution.VPP.push_back(i);
		// solution[vmidx]=i;
		// vm_list.erase(it);
		update_free_cap_pms(rcpu, rmem, rnet, i, backward_modification_traffic);
		// update_net_state(vm_list[j],i);
		//    it->pe = this->fit_func(*it,pm_set,vm_set);
		//   it->ke = 100;
		//  it->update();
		//  bool feasible=1;//feasibility_check(*it);
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
	cout << "total CPU requests of all VMs: " << total_cpu_req1;
	cout << endl
		 << "total Mem requests of all VMs: " << total_mem_req1;
	cout << endl
		 << "total BW requests of all VMs: " << total_net_req1;
	int num_act_pms = num_active_pms(vm_list_m);
	cout << endl;
	cout << "Active PM count (FFD): " << num_act_pms;
	/*cout << "solution:" <<endl;
	for(int i=0;i<num_vm_experiment;i++){
		cout<<solution[i]<<" ";
		mapping[i]=solution[i];
	}*/
	if (VERBOSE)
	{
		cout << endl
			 << "Free capacity of the PMs:" << endl;
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
		// this->Solution.freeCPU[i]=free_cap_pms[i];
	}
	/*
	int cpu_overload_pm[num_pm_experiment];
	int mem_overload_pm[num_pm_experiment];
	int net_overload_pm[num_pm_experiment];
	for(int i=0;i<num_pm_experiment;i++){
		cpu_overload_pm[i]=0;
		mem_overload_pm[i]=0;
		net_overload_pm[i]=0;
	}
	for(int i=0;i<num_vm_experiment;i++){
		this->Solution.freeCPU[i]=free_cap_pms[i];
		this->Solution.freeMEM[i]=free_cap_pms[i];
		this->Solution.freeCPU[i]=free_cap_pms[i];

	}
	for(int i=0;i<num_pm_experiment;i++) {
		cpu_overload_pm[i]=pm_list[i].capacity.cpu-free_cap_pms[i];
		mem_overload_pm[i]=pm_list[i].capacity.mem-free_cap_mem_pms[i];
		net_overload_pm[i]=pm_list[i].capacity.bw-free_cap_net_pms[i];
	}
	float server_pow=0;
	for(int i=0;i<num_pm_experiment;i++){
		if(cpu_overload_pm[i]!=0) server_pow+=1.0*pm_list[i].p_idle+1.0*(pm_list[i].p_max-pm_list[i].p_idle)*(cpu_overload_pm[i]/(1.0*pm_list[i].capacity.cpu));
		}
	float res_wastage=0;
	for(int i=0;i<num_pm_experiment;i++){	//for each PM, calculate the wasted resource and sum up for all PMs
		if(cpu_overload_pm[i]>0 and mem_overload_pm[i]>0){
					float L_cpu=(1.0*free_cap_pms[i])/(1.0*pm_list[i].capacity.cpu);
					float L_mem=(1.0*free_cap_mem_pms[i])/(1.0*pm_list[i].capacity.mem);
					float numerator=abs(L_cpu-L_mem)+epsilon_wastage;
					float denumerator=cpu_overload_pm[i]+mem_overload_pm[i];
					res_wastage+=numerator/denumerator;
		}
	}
	cout<<endl<<"Server Power consumption-FFD (Watts):"<<server_pow<<endl;
	float power_netw=0;//=find_power_net_pbr(pm_list,vm_list_m,vm_list,service_list,vm_to_vm_comm);
	cout<<endl<<"Net Power consumption-FFD (Watts):"<<power_netw<<endl;
	cout<<endl<<"Resource wastage of FFD:"<<res_wastage<<endl;
	active_pm_cnt_set(num_act_pms);
	powe_dis_set(server_pow+power_netw,server_pow,power_netw);
	res_wastage_set(res_wastage);
*/
	// calculate_balance();
	Solution.PValue = PVal;
	Solution.RmValue = RmVal;
	Solution.RpValue = RpVal;
	Solution.numActivePMs = num_act_pms;
	Solution.numVMs = vm_list_m.size();
	Solution.calculateMappingPower(pm_list);
	Solution.calculateMappingWastageAndBalance(pm_list);
	if (originalFFD)
		Solution.printMappingStatsIntoFile(fileName, pm_list_m);
}

ffd::ffd(vector<pm> pm_list_m, vector<vm> vm_list_m, int vm_to_vm_comm[][NUM_SERVICES])
{
	vector<pm> pm_list; //=pm_list_m;
	vector<vm> vm_list; //=vm_list_m;

	unsigned int vm_pm_mapping[vm_list_m.size()]; // element i containing x shows VMi is mapped to PMx
	for (unsigned int i = 0; i < vm_list_m.size(); i++)
		vm_pm_mapping[i] = pm_list_m.size();
	cout << endl
		 << "*********************************************************************";
	cout << endl
		 << "*********************************************************************";
	if (ffd_type == FFD_MEM)
		cout << endl
			 << "             FFD TYPE (sorting priority): MEM" << endl;
	else if (ffd_type == FFD_NET)
		cout << endl
			 << "             FFD TYPE (sorting priority): NET" << endl;
	else
		cout << endl
			 << "             FFD TYPE (sorting priority): CPU" << endl;
	cout << "***********************************************************************" << endl;
	cout << "**********************************************************************";
	long int total_cpu_req = 0;
	for (unsigned int ij = 0; ij < vm_list_m.size(); ij++)
		total_cpu_req += vm_list_m[ij].res.cpu;
	cout << endl
		 << "total CPU requests of all VMs (X): " << total_cpu_req << endl;
	for (vector<pm>::iterator it = pm_list_m.begin(); it != pm_list_m.end(); ++it)
	{
		pm_list.push_back(*it);
	}
	for (vector<vm>::iterator it = vm_list_m.begin(); it != vm_list_m.end(); ++it)
	{
		vm_list.push_back(*it);
	}
	for (unsigned int i = 0; i < vm_list_m.size(); i++)
	{
		solution[i] = vm_list_m.size();
		original_vm_indexes[i] = vm_list[i].id;
	}
	if (ffd_type == FFD_MEM)
		vm_list = sort_vms_decreasing_mem(vm_list);
	else if (ffd_type == FFD_NET)
		vm_list = sort_vms_decreasing_net(vm_list);
	else
		vm_list = sort_vms_decreasing_cpu(vm_list);
	initialize_free_cap_pms(pm_list);
	int vmidx = 0;
	// size_t x=pm_list_m.size();
	/*cout<<endl<<"Sorted VM list: ";
	for(vector<vm>::iterator it1=vm_list.begin();it1!=vm_list.end();it1++){
			cout<<it1->vm_id<<" ";
	}
	cout<<endl;*/
	int backward_modification_traffic = 0;
	for (unsigned int j = 0; j < vm_list_m.size(); j++) // vector<vm>::iterator it = vm_list.begin(); it != vm_list.end(); ++it)
	{
		// find the PM that can accommodate the next VM in the list
		unsigned int i = 0;
		unsigned int rcpu = vm_list[j].res.cpu; // it->requested_cpu;
		unsigned int rmem = vm_list[j].res.mem;
		unsigned int voi = vm_list[j].id;
		unsigned int rnet; //=find_pure_calc_vm_on_pm(vm_list[j],pm_list[i],vm_list,vm_to_vm_comm,service_list);//vm_list[j].requested_net;
		unsigned int rnet_total_vm = 0;
		// unsigned int pos_commun_vm=0;

		rnet = rnet_total_vm;
		// unsigned int comm_vm_idx=0;
		backward_modification_traffic = 0;
		if (rnet > 0) // if there are VMs previously mapped to the PM molecular[iii].pm_id, among them maybe there are commun VMs with vm_set[x1]
		{			  // if so, subtract their inter-commun BW from rnet (since they both located in the same PM)
		  // service ids may be utilized to decrease the loop length (vms of different services has no communication)
		  // for(vector<vm>::iterator it = service_list[vm_set[m.structure[j]].service_id].service_vms.begin(); it != service_list[vm_set[m.structure[j]].service_id].service_vms.end(); ++it){
		}
		while (free_cap_pms[i] < rcpu or free_cap_mem_pms[i] < rmem or (free_cap_net_pms[i] + backward_modification_traffic) < rnet)
		{ // !net_requirement_pass(vm_list[j],i,pm_list)){
			//	cout<<"insertion fail VM-"<<voi<< " into PM-"<<i<<"; Free Caps PM(C/M/N):"<<free_cap_pms[i]<<" "<<free_cap_mem_pms[i]<<" "<<free_cap_net_pms[i]+backward_modification_traffic<<" Req(C/M/N) "<<rcpu<<" "<<rmem<<" "<<rnet<<endl;

			i++;
			rnet = rnet_total_vm;
			// comm_vm_idx=0;
			backward_modification_traffic = 0;
			if (rnet > 0) // if there are VMs previously mapped to the PM molecular[iii].pm_id, among them maybe there are commun VMs with vm_set[x1]
			{			  // if so, subtract their inter-commun BW from rnet (since they both located in the same PM)
			  // service ids may be utilized to decrease the loop length (vms of different services has no communication)
			  // for(vector<vm>::iterator it = service_list[vm_set[m.structure[j]].service_id].service_vms.begin(); it != service_list[vm_set[m.structure[j]].service_id].service_vms.end(); ++it){
			  // comm_vm_idx=0;

				//}
			}
		}
		if (i >= sol.pm_in_sol.size())
		{
			if (i - sol.pm_in_sol.size() > 0)
				cout << endl
					 << "NOTE: there is at list one PM that can not host a single VM (PM: " << sol.pm_in_sol.size() << " ,VM: " << vm_list[j].id << ")";
			for (unsigned int n = sol.pm_in_sol.size(); n <= i; n++)
			{
				FFDone_pm_st new_pm;
				new_pm.pm_id = n;
				new_pm.num_vm = 0;
				sol.pm_in_sol.push_back(new_pm);
			}
		}
		sol.pm_in_sol[i].hosted_vms.push_back(vm_list[j].id);
		sol.pm_in_sol[i].num_vm++;
		solution[vm_list[j].id] = i;
		//	cout<<"VM-"<<voi <<" placed on PM-"<<i<<endl;
		vm_pm_mapping[voi] = i;
		vm_list[j].pm = i;
		// cout<<endl<<"assignment(VM->PM)"<<vm_list[j].vm_id<<"->"<<i;
		// solution[vmidx]=i;
		// vm_list.erase(it);
		update_free_cap_pms(rcpu, rmem, rnet, i, backward_modification_traffic); //******note that the commun vol of those VMs communicating with vm j should also be modified
		// update_net_state(vm_list[j],i);
		//    it->pe = this->fit_func(*it,pm_set,vm_set);
		//   it->ke = 100;
		//  it->update();
		//  bool feasible=1;//feasibility_check(*it);
		vmidx++;
	}
	long int total_cpu_req1 = 0;
	for (unsigned int ij = 0; ij < vm_list_m.size(); ij++)
		total_cpu_req1 += vm_list[ij].res.cpu;
	cout << "total CPU requests of all VMs: " << total_cpu_req1;
	unsigned int num_act_pms = num_active_pms(vm_list);
	cout << endl;
	cout << "Active PM count (FFD): " << num_act_pms;
	/*cout << "solution:" <<endl;
	for(int i=0;i<num_vm_experiment;i++){
		cout<<solution[i]<<" ";
		mappingSol[i]=solution[i];
	}*/
	cout << endl
		 << "Free capacity of the PMs:" << endl;
	for (int i = 0; i < num_pm_experiment; i++)
	{
		cout << free_cap_pms[i] << " ";
	}
	cout << endl;
	unsigned int cpu_overload_pm[num_pm_experiment];
	unsigned int mem_overload_pm[num_pm_experiment];
	// unsigned int net_overload_pm[num_pm_experiment];
	for (int i = 0; i < num_pm_experiment; i++)
	{
		cpu_overload_pm[i] = 0;
		mem_overload_pm[i] = 0;
		// net_overload_pm[i]=0;
	}
	for (int i = 0; i < num_pm_experiment; i++)
	{
		cpu_overload_pm[i] = pm_list[i].capacity.cpu - free_cap_pms[i];
		mem_overload_pm[i] = pm_list[i].capacity.mem - free_cap_mem_pms[i];
		// net_overload_pm[i]=pm_list[i].capacity.bw-free_cap_net_pms[i];
	}
	float server_pow = 0;
	for (int i = 0; i < num_pm_experiment; i++)
	{
		if (cpu_overload_pm[i] != 0)
			server_pow += 1.0 * pm_list[i].p_idle + 1.0 * (pm_list[i].p_max - pm_list[i].p_idle) * (1.0 * cpu_overload_pm[i] / (1.0 * pm_list[i].capacity.cpu));
	}
	float res_wastage = 0;
	for (int i = 0; i < num_pm_experiment; i++)
	{ // for each PM, calculate the wasted resource and sum up for all PMs
		if (cpu_overload_pm[i] > 0 and mem_overload_pm[i] > 0)
		{
			float L_cpu = (1.0 * free_cap_pms[i]) / (1.0 * pm_list[i].capacity.cpu);
			float L_mem = (1.0 * free_cap_mem_pms[i]) / (1.0 * pm_list[i].capacity.mem);
			float numerator = abs(L_cpu - L_mem) + epsilon_wastage;
			float denumerator = cpu_overload_pm[i] + mem_overload_pm[i];
			res_wastage += numerator / denumerator;
		}
	}
	cout << endl
		 << "Server Power consumption-FFD (Watts):" << server_pow << endl;
	float power_netw = 0; // find_power_net_pbr(pm_list,vm_list_m,vm_list,service_list);
	cout << endl
		 << "Net Power consumption-FFD (Watts):" << power_netw << endl;

	cout << "Resource wastage of FFD:" << res_wastage << endl;

	active_pm_cnt_set(num_act_pms);
	powe_dis_set(server_pow + power_netw, server_pow, power_netw);
	res_wastage_set(res_wastage);
	// calculate_balance();
}

vector<vm> ffd::sort_vms_decreasing_cpu(vector<vm> vm_list)
{
	// sort the VMs in decreasing CPU demand order (then memory)
	for (unsigned int i = 0; i < vm_list.size(); i++)
	{
		for (unsigned int j = i + 1; j < vm_list.size(); j++)
		{
			unsigned int tmp1 = vm_list[i].res.cpu;
			unsigned int tmp2 = vm_list[j].res.cpu;
			unsigned int tmp3 = vm_list[i].res.mem;
			unsigned int tmp4 = vm_list[j].res.mem;

			if (tmp1 < tmp2)
			{
				swap(vm_list[i], vm_list[j]);
			}
			else if (tmp1 == tmp2)
			{
				if (tmp3 < tmp4)
				{
					swap(vm_list[i], vm_list[j]);
				}
				/*}
				else if(tmp1==tmp2 and tmp3==tmp4){
					if(tmp5<tmp6){
							swap(vm_list[i],vm_list[j]);
							}*/
			}
		}
	}
	// sort(vm_list.begin(), vm_list.end(), greater<int>());
	/*for(int i=0;i<num_vm_experiment;i++){
		cout<<vm_list[i].requested_cpu<<" ";
	}*/
	// cout<<endl;
	/*for(int i=0;i<num_vm_experiment;i++){
			cout<<original_vm_indexes[i]<<" ";
		}
		cout<<endl;*/
	return vm_list;
}

vector<vm> ffd::sort_vms_decreasing_mem(vector<vm> vm_list)
{
	// sort the VMs in decreasing CPU demand order (then memory)
	for (unsigned int i = 0; i < vm_list.size(); i++)
	{
		for (unsigned int j = i + 1; j < vm_list.size(); j++)
		{
			unsigned int tmp1 = vm_list[i].res.cpu;
			unsigned int tmp2 = vm_list[j].res.cpu;
			unsigned int tmp3 = vm_list[i].res.mem;
			unsigned int tmp4 = vm_list[j].res.mem;
			unsigned int tmp5 = vm_list[i].res.bw;
			unsigned int tmp6 = vm_list[j].res.bw;
			if (tmp3 < tmp4)
			{
				swap(vm_list[i], vm_list[j]);
			}
			else if (tmp3 == tmp4)
			{
				if (tmp1 < tmp2)
				{
					swap(vm_list[i], vm_list[j]);
				}
			}
			else if (tmp1 == tmp2 and tmp3 == tmp4)
			{
				if (tmp5 < tmp6)
				{
					swap(vm_list[i], vm_list[j]);
				}
			}
		}
	}
	return vm_list;
}

vector<vm> ffd::sort_vms_decreasing_net(vector<vm> vm_list)
{
	// sort the VMs in decreasing CPU demand order (then memory)
	for (unsigned int i = 0; i < vm_list.size(); i++)
	{
		for (unsigned int j = i + 1; j < vm_list.size(); j++)
		{
			unsigned int tmp1 = vm_list[i].res.cpu;
			unsigned int tmp2 = vm_list[j].res.cpu;
			unsigned int tmp3 = vm_list[i].res.mem;
			unsigned int tmp4 = vm_list[j].res.mem;
			unsigned int tmp5 = vm_list[i].res.bw;
			unsigned int tmp6 = vm_list[j].res.bw;
			if (tmp5 < tmp6)
			{
				swap(vm_list[i], vm_list[j]);
			}
			else if (tmp5 == tmp6)
			{
				if (tmp1 < tmp2)
				{
					swap(vm_list[i], vm_list[j]);
				}
			}
			else if (tmp1 == tmp2 and tmp5 == tmp6)
			{
				if (tmp3 < tmp4)
				{
					swap(vm_list[i], vm_list[j]);
				}
			}
		}
	}
	return vm_list;
}

void ffd::sort_pms_decreasing_cpu(vector<pm> pm_list)
{
}

void ffd::analyzeMapping()
{
}
