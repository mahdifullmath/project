/*
 * vm.cpp
 *
 *  Created on: Oct 2, 2023
 *      Author: mkiani
 */

#include "vm.h"
#include <random>
#include <cstdlib>

using namespace std;

vm::vm()
{
}

vm::~vm()
{
}

void vm::init_vm(int t_vm, int csID, int dcID, int vmID,
                 VMresource Res, VMutilization Util, VMutilization Reven,
                 int t_start, int t_finish, const int initial_pm_id)
{
    sid = csID;
    id = vmID;
    pm = initial_pm_id;
}

vm vm::vm_generator_corr(int vm_id, int serv_id, float Rp, float Rm, float P, int num_pms, int req_cpu_max, int req_mem_max)
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
    newvm.pm = num_pms; // no PM for the time being
    // cout<<endl<< tmp1<<"/  "<<tmp2;
    return newvm;
}

vector<vm> vm::experiment_vm(vector<vm> vm_set, int n_experiment, int num_vms)
{
    // create vms
    // create_vm_set(num_vm_experiment,req_cpu_min,req_cpu_max,req_mem_min,req_mem_max);
    vm newvm;
    for (int i = 0; i < num_vms; i++)
    {
        //	newvm=vm_generator(i,req_cpu_min,req_cpu_max,req_mem_min,req_mem_max);
        //	newvm=vm_generator_corr(i,Rp,Rm,P_array[n_experiment]);
        vm_set.push_back(newvm);
    }
    return vm_set;
}

void vm::fillNullValues()
{
    // put invalid numbers in the VM
    res.bw = 0;
    res.cpu = 0;
    res.mem = 0;
    id = 1000000;
    sid = 1000000;
    pm = 1000000;
}

int rand_lo_high(int low, int high)
{
    if (high > low)
        return low + rand() % (high - low);
    else
        return low;
}

vm vm::vm_generator(int vm_id, int cpu_req_l, int cpu_req_h, int mem_req_l, int mem_req_h)
{
    vm newvm;
    newvm.res.cpu = rand_lo_high(cpu_req_l, cpu_req_h);
    newvm.res.mem = rand_lo_high(mem_req_l, mem_req_h);
    newvm.id = vm_id;
    // newvm.pm = num_pm_experiment;
    return newvm;
}
