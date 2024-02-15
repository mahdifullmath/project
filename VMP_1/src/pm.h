/*
 * pm.h
 *
 *  Created on: Oct 2, 2023
 *      Author: mkiani
 */

#ifndef INC_PM_H_
#define INC_PM_H_

#include <vector>
// #include "commonDefs.h"

using namespace std;

struct PMresource
{
    float cpu;
    float mem;
    float bw;
};

struct PMutilization
{
    float cpu;
    float mem;
    float bw;
};

class pm
{
public:
    pm();
    virtual ~pm();
    bool state;
    unsigned int id;
    unsigned int dc;   // datacenter id
    unsigned int rack; // rack id
    unsigned int type;
    unsigned int row;
    // int cap_cpu,cap_mem,cap_net,cap_store;
    unsigned int p_max, p_idle;
    unsigned int num_vms; //,total_requested_cpu,total_requested_mem,total_requested_net;
    // float util_cpu,util_mem,util_net,util_storage,util_total;
    PMresource capacity;
    PMutilization util;
    PMutilization forec_pm_utilization;
    int predicted_state; // 0 normal, 1 underload, 2 overload
    void set_state(bool state);
    void initialize(bool state, int pm_id, int dc_id, int rack_id, int pm_type_index, PMresource init_res, int p_idle_pm, int p_max_pm);
    void set_utilization(PMutilization new_resources);
    void initialize_routes_pm(unsigned int pm_id);
    void predict_cpu_util_mv(); // use Moving average to predict utilization (CPU) of VMs hence the PM in the next
    void calculateAndSetPMUtils();
    int *hosted_vms;
    int num_mig_time_in, num_mig_time_out;
    int total_mig_in, total_mig_out;

    // vector<moving_average> mv_predictor;
    //        vector<int> hosted_vms;
protected:
private:
};

#endif /* INC_PM_H_ */
