/*
 * WF.h
 *
 *  Created on: Oct 19, 2023
 *      Author: mkiani
 */

#ifndef WF_H_
#define WF_H_

#include <iostream>
#include <bits/stdc++.h>
#include <iostream>
#include <vector>

#include "defs.h"
#include "mapping.h"
#include "idata.h"

// #include "utilityFFD.h"

using namespace std;

struct WFone_pm_st
{
    vector<int> hosted_vms; //
    int pm_id;              //
    int num_vm;
    float fit_pm;
    int requested_net;
};

struct WFsolution_st
{
    vector<WFone_pm_st> pm_in_sol; //
    float fit_sol;
    int requested_net;
};

// first shuffle the VM list randomly, then insert each VM into the first PM with sufficient remaining space.
class WF // best-fit decreasing
{
public:
    WF();
    WF(vector<pm> pm_list, vector<vm> vm_list, float RpVal, float RmVal, float PVal, std::ofstream &fileName, bool originalWF, unsigned int iterationForAvg); //,int vm_to_vm_comm[][NUM_SERVICES*MAX_VM_PER_SERVICE]);
    virtual ~WF();
    void initialize();
    unsigned int findWorstPM(vm theVM, vector<pm> PMList);
    int *hosted_vms;
    int mappingSol[num_vm_experiment];
    int num_active_pm;
    mapping Solution;
    float pow_dis;
    float pow_dis_net;
    float pow_dis_server;
    float commun_vol;
    float resource_wastage;

    unsigned int solution[NUM_SERVICES * MAX_VM_PER_SERVICE]; // each element shows which PM hosts the VM (index=VM, value=PM)
    unsigned int free_cap_pms[num_pm_experiment];
    unsigned int free_cap_mem_pms[num_pm_experiment];
    unsigned int free_cap_net_pms[num_pm_experiment];
    unsigned int original_vm_indexes[NUM_SERVICES * MAX_VM_PER_SERVICE];
    WFsolution_st sol;
    void active_pm_cnt_set(int ap) { num_active_pm = ap; }
    void powe_dis_set(float pd_total, float pd_server, float pd_net) { pow_dis = pd_total, pow_dis_server = pd_server, pow_dis_net = pd_net; }
    void commun_vol_set(float commun_v) { commun_vol = commun_v; }
    void res_wastage_set(float rw) { resource_wastage = rw; }
    void analyzeMapping(); // to calculate power energy and wasted resources of the generated mapping solution
    void initialize_free_cap_pmsWF(vector<pm> pm_list);
    void update_free_cap_pmsWF(int cap_cpu, int cap_mem, int cap_net, int pm, int backward_modification_traffic);
    int num_active_pmsWF(vector<vm> vm_list);

protected:
private:
};

#endif /* WF_H_ */
