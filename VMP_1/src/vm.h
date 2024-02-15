/*
 * vm.h
 *
 *  Created on: Oct 2, 2023
 *      Author: mkiani
 */

#ifndef VM_H_
#define VM_H_

#include <vector>
// using namespace std;

struct VMresource
{
    float cpu;
    float mem;
    float bw;
};

struct VMutilization
{
    float cpu;
    float mem;
    float bw;
};

class vm
{
public:
    vm();
    vm(int *pf);
    virtual ~vm();
    unsigned int id;
    unsigned int pm;
    unsigned int sid; // service id
    VMresource res;
    VMutilization util;
    void init_vm(int t_vm, int csID, int dcID, int vmID, VMresource Res, VMutilization Util, VMutilization Reven, int t_start, int t_finish, const int initial_pm_id);
    // vm  vm_generator_corr(int vm_id, int serv_id, float Rp, float Rm, float P,int num_pms);
    vm vm_generator_corr(int vm_id, int serv_id, float Rp, float Rm, float P, int num_pms, int req_cpu_max, int req_mem_max);
    vm vm_generator(int vm_id, int cpu_req_l, int cpu_req_h, int mem_req_l, int mem_req_h);
    std::vector<vm> experiment_vm(std::vector<vm> vm_set, int n_experiment, int num_vms);
    void fillNullValues(); // put null and invalid values in the vm

    //    vector<int> hosted_vms;
protected:
private:
};

#endif /* VM_H_ */
