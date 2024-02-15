/*
 * idata.h
 *
 *  Created on: Oct 2, 2023
 *      Author: mkiani
 */

#ifndef IDATA_H_
#define IDATA_H_

#include "defs.h"
#include "pm.h"
// #include "service.h"
#include "vm.h"

class idata
{
public:
    idata();
    virtual ~idata();
    vector<pm> iPMs;
    int experimentID;
    // vector<service> iServices;
    vector<vm> iVMs;
    // service tempServ;
    vm tempVM;
    void createPMs(unsigned int num_pm_experiment);
    void createVMs(unsigned int num_VM_experiment);
    void createServices(unsigned int numServ, unsigned int MinNumVMperServ, int MaxNumVMperServ, int experimentID);
    // service iCreateOneService(int MIN_VM_PER_SERV,int MAX_VM_PER_SERV,int sid,int experimentID);
    void experiment_pm(int n_experiment, int NUM_DC_PER_CLOD, int NUM_PM_PER_RAK, int num_pm_typs);
    void experiment_vm(int num_vms);
    pm pm_generator_model(int local_pmid, int unique_pm_id, int rowid, int rack_id, int dcid, int cpu_cap_l, int cpu_cap_h, int mem_cap_l, int mem_cap_h, int num_pm_typs);
    void print_pm_set_with_type(vector<pm> pm_set);
    void printVMs();
    void printPMs();
    void printPMsWithPower();
    // void printServices();
protected:
private:
};

#endif /* IDATA_H_ */
