/*
 * ffd.h
 *
 *  Created on: Oct 2, 2023
 *      Author: mkiani
 */

#ifndef FFD_H_
#define FFD_H_

#include <iostream>
#include <bits/stdc++.h>
#include <iostream>
#include <vector>

#include "defs.h"
#include "mapping.h"
#include "idata.h"

// #include "utilityFFD.h"

using namespace std;

struct FFDone_pm_st
{
    vector<int> hosted_vms; //
    int pm_id;              //
    int num_vm;
    float fit_pm;
    int requested_net;
};

struct FFDsolution_st
{
    vector<FFDone_pm_st> pm_in_sol; //
    float fit_sol;
    int requested_net;
};

// first sort the VMs to be inserted in decreasing CPU demand order, and then inserting each VM into the first PM with sufficient remaining space.
class ffd // best-fit decreasing
{
public:
    ffd();
    ffd(vector<pm> pm_list, vector<vm> vm_list, float RpVal, float RmVal, float PVal, std::ofstream &fileName, bool originalFFD, unsigned int iterationForAvg); //,int vm_to_vm_comm[][NUM_SERVICES*MAX_VM_PER_SERVICE]);
    ffd(vector<pm> pm_list, vector<vm> vm_list, int x);
    // ffd(vector <pm> pm_list,vector <vm> vm_list,vector<service> service_list);//,int vm_to_vm_comm[][NUM_SERVICES*MAX_VM_PER_SERVICE]);
    // ffd(vector <pm> pm_list,vector <vm> vm_list,vector<service> service_list,int vm_to_vm_comm[][NUM_SERVICES]);
    ffd(vector<pm> pm_list_m, vector<vm> vm_list_m, int vm_to_vm_comm[][NUM_SERVICES]);

    //   pm(int *pf);
    virtual ~ffd();
    void initialize();
    vector<vm> sort_vms_decreasing_cpu(vector<vm> vm_list);
    void sort_pms_decreasing_cpu(vector<pm> pm_list);
    vector<vm> sort_vms_decreasing_mem(vector<vm> vm_list);
    vector<vm> sort_vms_decreasing_net(vector<vm> vm_list);
    int *hosted_vms;
    int mappingSol[num_vm_experiment];
    int num_active_pm;
    mapping Solution;
    float pow_dis;
    float pow_dis_net;
    float pow_dis_server;
    float commun_vol;
    float resource_wastage;
    FFDsolution_st sol;
    void active_pm_cnt_set(int ap) { num_active_pm = ap; }
    void powe_dis_set(float pd_total, float pd_server, float pd_net) { pow_dis = pd_total, pow_dis_server = pd_server, pow_dis_net = pd_net; }
    void commun_vol_set(float commun_v) { commun_vol = commun_v; }
    void res_wastage_set(float rw) { resource_wastage = rw; }
    void analyzeMapping(); // to calculate power energy and wasted resources of the generated mapping solution
    //        vector<int> hosted_vms;
protected:
private:
};

#endif /* FFD_H_ */
