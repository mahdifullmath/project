/*
 * idata.cpp
 *
 *  Created on: Oct 2, 2023
 *      Author: mkiani
 */

#include "idata.h"
#include <iostream>

using namespace std;

idata::idata()
{
}

idata::~idata()
{
}

const int cap_cpu_min = 100; // mips
const int cap_cpu_max = 100;
const int cap_mem_min = 100; // gb
const int cap_mem_max = 100;

const int pm_model_cpu[4] = {100, 200, 400, 600};
const int pm_model_mem[4] = {100, 200, 400, 600};
const int pm_model_net[4] = {45000, 60000, 96000, 120000}; // 96Gbps

const int port_tor_power = 9;    // each port of TOR switch consumes x Watts (9 W/10GbE interface)
const int port_aggreg_power = 9; // each port of Aggregation switch consumes x Watts (36 watt for each 40 Gbps router port)
const int port_core_power = 36;  // each port of Core switch consumes x Watts (36 watt for each 40 Gbps router port)

const int cpu_p_idle[4] = {100, 150, 175, 200};
const int cpu_p_max[4] = {145, 214, 250, 285};
#define NUM_VM_TYPES 4
const int req_cpu_min = 40; // mips
const int req_cpu_max = 45;
const int req_mem_min = 40; // gb
const int req_mem_max = 45;

void idata::createPMs(unsigned int num_pm_experiment)
{
    // create PMs for the algorithms
    vector<pm> PMsofAlg;
    this->experiment_pm(num_pm_experiment, 1, 1000, NUM_VM_TYPES);
}

void idata::createVMs(unsigned int num_VM_experiment)
{
    // create PMs for the algorithms
    vector<vm> VMsofAlg;
    this->experiment_vm(num_VM_experiment);
}

vm vm_generator_corr2(int vm_id, int serv_id, float Rp, float Rm, float P, int req_cpu_max, int req_mem_max)
{
    /*1. For i = 1 to n do
    2. Rpi = rand (2Rp) ;
    3. Rmi = rand (Rm) ;
    4. r = rand(1.0) ;
    5. If (r < P  Rpi >= Rp)  (r>=P  Rpi < Rp)
    6. 	Rmi = Rmi + Rm;
    8. End For*/
    // case 1: Rp=Rm=0.25; case2: Rp=Rm=0.45 (both cases with P = 0.00, 0.25, 0.50, 0.75, and 1.0).the average correlation coefficients should then be calculated
    // gen rand between Lo and Hi: float r3 = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));

    float Rpi = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (2.0 * Rp)));
    float Rmi = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (Rm)));
    float r = (float)rand() / RAND_MAX;
    if ((r < P and Rpi >= Rp) or (r >= P and Rpi < Rp))
    {
        Rmi = Rmi + Rm;
    }
    vm newvm;
    int tmp1 = 10 + (int)(Rpi * req_cpu_max);
    int tmp2 = 10 + (int)(Rmi * req_mem_max);
    newvm.res.cpu = tmp1;
    newvm.res.mem = tmp2;
    newvm.id = vm_id;
    newvm.sid = serv_id;
    newvm.pm = 1000000; // no PM for the time being
    // cout<<endl<< tmp1<<"/  "<<tmp2;
    return newvm;
}

void idata::experiment_vm(int num_vms)
{
    int uniqu_vmid = 0;
    // pm newpm ;
    // for(int i=0;i<num_pm_experiment;i++){
    for (unsigned int vid = 0; vid < num_vms; vid++)
    {
        iVMs.push_back(vm_generator_corr2(vid, 0, Rp, Rm, P_array[this->experimentID], req_cpu_max, req_mem_max)); // vid,sid(0),Rp,Rm.P,maxCPU,maxMEM
        uniqu_vmid++;
    }
    // int num_pms = iPMs.size();
}

void idata::experiment_pm(int num_pms, int NUM_DC_PER_CLOD, int NUM_PM_PER_RAK, int num_pm_typs)
{

    int rack_id = 0;
    int pm_cnt = 0;
    int uniqu_pid = 0;
    // pm newpm ;
    // for(int i=0;i<num_pm_experiment;i++){
    for (unsigned int pid = 0; pid < num_pms; pid++)
    {
        pm thePM;
        thePM = pm_generator_model(pm_cnt, uniqu_pid, 0, 0, 0, cap_cpu_min, cap_cpu_max, cap_mem_min, cap_mem_max, num_pm_typs);
        iPMs.push_back(thePM);
        // newpm = pm_generator_model(pm_cnt, uniqu_pid, rowid, rkid, dcid, cap_cpu_min, cap_cpu_max, cap_mem_min, cap_mem_max, num_pm_typs);
        pm_cnt++;
        uniqu_pid++;
        if (pm_cnt == NUM_PM_PER_RAK)
        {
            rack_id++;
            pm_cnt = 0;
        }
    }
    // int num_pms = iPMs.size();
}

int rand_lo_high2(int low, int high)
{
    if (high > low)
        return low + rand() % (high - low);
    else
        return low;
}

pm idata::pm_generator_model(int local_pmid, int unique_pm_id, int rowid, int rack_id, int dcid, int cpu_cap_l,
                             int cpu_cap_h, int mem_cap_l, int mem_cap_h, int num_pm_typs)
{
    pm anewPM;
    int pm_model = rand_lo_high2(0, num_pm_typs);
    anewPM.capacity.cpu = pm_model_cpu[pm_model];
    anewPM.capacity.mem = pm_model_mem[pm_model];
    anewPM.capacity.bw = pm_model_net[pm_model];
    anewPM.p_idle = cpu_p_idle[pm_model];
    anewPM.p_max = cpu_p_max[pm_model];
    anewPM.num_vms = 0;
    anewPM.type = pm_model;
    // newpm.state=1;
    anewPM.id = unique_pm_id;
    anewPM.rack = rack_id;
    anewPM.dc = dcid;
    anewPM.row = rowid;
    /*newpm.total_requested_cpu=0;
    newpm.total_requested_mem=0;
    newpm.total_requested_net=0;
    newpm.util_cpu=0;
    newpm.util_mem=0;
    newpm.util_net=0;
    newpm.util_total=0;*/
    return anewPM;
}

void idata::print_pm_set_with_type(vector<pm> pm_set)
{
    cout << endl
         << "PMs: ";
    for (vector<pm>::iterator it_1 = pm_set.begin(); it_1 != pm_set.end(); it_1++)
    {
        cout << endl
             << "PM-" << it_1->id << " Type: " << it_1->type << " Caps (CPU Mem Net): " << it_1->capacity.cpu << " " << it_1->capacity.mem << " " << it_1->capacity.bw;
    }
    cout << endl;
}

void idata::printPMs()
{
    cout << endl
         << "PMs: ";
    for (vector<pm>::iterator it_1 = iPMs.begin(); it_1 != iPMs.end(); it_1++)
    {
        cout << endl
             << "PM-" << it_1->id << " Type: " << it_1->type << " Caps (CPU Mem Net): " << it_1->capacity.cpu << " " << it_1->capacity.mem << " " << it_1->capacity.bw;
    }
    cout << endl;
}

void idata::printPMsWithPower()
{
    cout << endl
         << "PMs: ";
    for (vector<pm>::iterator it_1 = iPMs.begin(); it_1 != iPMs.end(); it_1++)
    {
        cout << endl
             << "PM-" << it_1->id << " Type: " << it_1->type << " Caps (CPU Mem Net): " << it_1->capacity.cpu << " " << it_1->capacity.mem << " " << it_1->capacity.bw
             << " power (idle | max): " << it_1->p_idle << " | " << it_1->p_max;
    }
    cout << endl;
}

/*
void idata::  printServices(){
     cout << endl
         << "Services: ";
    for (vector<service>::iterator it_1 = iServices.begin(); it_1 != iServices.end(); it_1++)
    {
        cout <<"Service "<< endl;
            for (vector<vm>::iterator it_2 = it_1->service_vms.begin(); it_2 != it_1->service_vms.end(); it_2++){
                cout<<" "<<it_2->id;
            }

    }
    cout << endl;
}*/

void idata::printVMs()
{
    cout << endl;
    for (vector<vm>::iterator it_1 = iVMs.begin(); it_1 != iVMs.end(); it_1++)
    {
        cout << "VM " << it_1->id << "(cpu/mem/bw): " << it_1->res.cpu << "| " << it_1->res.mem << "| " << it_1->res.bw;
        cout << endl;
    }
    cout << endl;
}
