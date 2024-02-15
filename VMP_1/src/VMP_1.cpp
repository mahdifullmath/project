/*
 ============================================================================
 Name        : VMP_1.c
 Author      : M Kiani
 Version     :
 Copyright   : Your copyright notice
 Description : This code in this current version only implements FFD in a static VMP problem.
 the parameters of a simulation should be set within the"defs.h" header file
 simulation settings includes:
  Rm,Rp,P of random generator,
  #VMs, #PMs

  (if necessary):
  resource capacity values of PMs,
  number of PM types, P_idle and P_max of the PMs,
  resource request of VMs
  name of the output folder


 NOTEs (for windows):
 1- install MinGW and select it when creating the project
 2- set PATH in Env. variables: C:\MinGW\msys\1.0\bin,    C:\MinGW\bin
 3- in project settings-> project natures  add C++ Nature
 ============================================================================
 */
#include <iostream>
#include <sys/time.h>
#include <fstream>
#include <string>
#include <cmath>
#include <limits>
#include <climits>
#include <bits/stdc++.h>
// #include "time.h"
// #include "include/service.h"
// #include "include/utility.h"
// #include "include/ffd.h"
#include "idata.h"
#include "defs.h"
#include "ffd.h"
#include "RF.h"
#include "BF.h"
#include "WF.h"
#include "ga.h"
#include "Hho.h"
// #include "include/defenitions.h"
// #include "include/pm.h"
// #include "vm.h"

using namespace std;
#define FOLDER "results"
#define ALG1 "FFD"
#define ALG3 "BF"
#define ALG4 "WF"
#define ALG5 "RF"
#define ALG6 "GA"
#define ALG7 "HHO"
#define NUM_ALGS 7
#define NUM_PARAMERTERS 6 // POWER, WASTAGE   NumActivePMs  UtilCPU UtilMEM ExeTime
#define NUM_ITERATION NUM_ITERATIONS_FOR_AVG

string parameterNames[NUM_PARAMERTERS] = {"Power", "Wastage", "numActivePMs", "utilCPU", "utilMEM", "tExecution"};
string algNames[NUM_ALGS] = {" FFD ", " RF ", " BF", " WF ", " GA "};

float resultsAllAlgorithmsAllIterations[NUM_ALGS][NUM_PARAMERTERS][NUM_ITERATION];

// vector<pm> pm_list;
// vector<vm> vm_list;
// vector<service> service_list;

const float P_array2[5] = {0.00, 0.25, 0.50, 0.75, 1.0};
// TODO: add load balance as an additional evaluation parameter

void saveResultsofIteration(unsigned int iterationForAvg, unsigned int algIndex, float power, float wastage, float numActivePMs, float utilCPU, float utilMEM, float tExecution)
{
	resultsAllAlgorithmsAllIterations[algIndex][0][iterationForAvg] = power;
	resultsAllAlgorithmsAllIterations[algIndex][1][iterationForAvg] = wastage;
	resultsAllAlgorithmsAllIterations[algIndex][2][iterationForAvg] = numActivePMs;
	resultsAllAlgorithmsAllIterations[algIndex][3][iterationForAvg] = utilCPU;
	resultsAllAlgorithmsAllIterations[algIndex][4][iterationForAvg] = utilMEM;
	resultsAllAlgorithmsAllIterations[algIndex][5][iterationForAvg] = tExecution;
}

void printAllResultsInAFile()
{
	char name_of[100];
	ofstream out_file_alg;
	string str_of1;
	string nvmstr = std::to_string(num_vm_experiment);
	string pselstr = std::to_string(EXPERIMENT_P_VALUE_SELECTOR);
	sprintf(name_of, "%s/RES_%s_%sVMs_%sPvalue.txt", FOLDER, "ALL", nvmstr.c_str(), pselstr.c_str());
	str_of1 = name_of;
	out_file_alg.open(name_of, std::ofstream::out | std::ofstream::trunc);
	out_file_alg.close();
	out_file_alg.open(str_of1, fstream::app);

	out_file_alg << "the results of all algorithms in all iterations for one VMCount and rho" << endl;

	for (unsigned int k = 0; k < NUM_ITERATIONS_FOR_AVG; k++)
	{
		out_file_alg << endl
					 << endl
					 << "run-" << k << endl
					 << endl;
		for (unsigned int j = 0; j < NUM_PARAMERTERS; j++)
		{
			out_file_alg << parameterNames[j] << ":" << endl;
			for (unsigned int i = 0; i < NUM_ALGS; i++)
				out_file_alg << algNames[i] << "        ";
			out_file_alg << endl;
			for (unsigned int i = 0; i < NUM_ALGS; i++)
			{
				out_file_alg << resultsAllAlgorithmsAllIterations[i][j][k] << "\t";
			}
			out_file_alg << endl
						 << endl;
		}
	}
}

// vector<service> service_list;
int runGA(idata PMserv, unsigned int iterationForAvg, unsigned int algIndex)
{
	char name_of[100];
	ofstream out_file_alg;
	string str_of1;
	string nvmstr = std::to_string(num_vm_experiment);
	string pselstr = std::to_string(EXPERIMENT_P_VALUE_SELECTOR);
	sprintf(name_of, "%s/RES_%s_%sVMs_%sPvalue.txt", FOLDER, ALG6, nvmstr.c_str(), pselstr.c_str());
	str_of1 = name_of;

	if (iterationForAvg == 0)
	{
		out_file_alg.open(name_of, std::ofstream::out | std::ofstream::trunc);
		out_file_alg.close();
	}
	out_file_alg.open(str_of1, fstream::app);
	struct timeval st_t, end_t;
	std::cout << endl
			  << "=======================GA=======================" << endl;

	gettimeofday(&st_t, NULL);
	std::cout << " running GA algorithm" << std::endl;

	ga hho1(GA_epoch, GA_pop_size, GA_num_dims);
	hho1.initialize(PMserv.iVMs);
	if (VERBOSE)
		hho1.printPop();
	std::cout << "initialization of GA pop is finished!" << std::endl;
	if (VERBOSE)
		PMserv.printPMsWithPower();
	hho1.evolve(PMserv.iPMs, PMserv.iVMs, out_file_alg, iterationForAvg);
	std::cout << endl
			  << "===================END GA======================" << endl;

	gettimeofday(&end_t, NULL);
	double ts = st_t.tv_sec * 1.0e6 + (double)st_t.tv_usec;
	double te = end_t.tv_sec * 1.0e6 + (double)end_t.tv_usec;
	double tExecution = te - ts;
	std::cout << "GA Execution time(us): " << tExecution << endl;
	out_file_alg << "GA Execution time(us): " << tExecution << endl;
	out_file_alg << hho1.Solution.power << "    " << hho1.Solution.wastage << "    " << hho1.Solution.numActivePMs << "    " << hho1.Solution.utilCPU << "    " << hho1.Solution.utilMEM << "    " << tExecution << endl;
	/*resultsAllAlgorithmsAllIterations[algIndex][0][iterationForAvg]=hho1.Solution.power;
	resultsAllAlgorithmsAllIterations[algIndex][1][iterationForAvg]=hho1.Solution.wastage;
	resultsAllAlgorithmsAllIterations[algIndex][2][iterationForAvg]=hho1.Solution.numActivePMs;
	resultsAllAlgorithmsAllIterations[algIndex][3][iterationForAvg]=hho1.Solution.utilCPU;
	resultsAllAlgorithmsAllIterations[algIndex][4][iterationForAvg]=hho1.Solution.utilMEM;
	resultsAllAlgorithmsAllIterations[algIndex][5][iterationForAvg]=(float) tExecution;
	*/
	saveResultsofIteration(iterationForAvg, algIndex, hho1.Solution.power, hho1.Solution.wastage, hho1.Solution.numActivePMs, hho1.Solution.utilCPU, hho1.Solution.utilMEM, tExecution);

	return 0;
}

void runFFD(idata PMserv, unsigned int iterationForAvg, unsigned int algIndex)
{
	char name_of[100];
	ofstream out_file_alg1;
	string str_of1;
	string nvmstr = std::to_string(num_vm_experiment);
	string pselstr = std::to_string(EXPERIMENT_P_VALUE_SELECTOR);
	sprintf(name_of, "%s/RES_%s_%sVMs_%sPvalue.txt", FOLDER, ALG1, nvmstr.c_str(), pselstr.c_str());
	str_of1 = name_of;

	if (iterationForAvg == 0)
	{
		out_file_alg1.open(name_of, std::ofstream::out | std::ofstream::trunc);
		out_file_alg1.close();
	}
	out_file_alg1.open(str_of1, fstream::app);
	struct timeval st_t, end_t;
	std::cout << endl
			  << "==============FFD====================";

	gettimeofday(&st_t, NULL);
	ffd ffd1(PMserv.iPMs, PMserv.iVMs, Rp, Rm, P_array2[EXPERIMENT_P_VALUE_SELECTOR], out_file_alg1, 1, iterationForAvg);
	std::cout << endl
			  << "==============END FFD================" << endl;

	gettimeofday(&end_t, NULL);
	double ts = st_t.tv_sec * 1.0e6 + (double)st_t.tv_usec;
	double te = end_t.tv_sec * 1.0e6 + (double)end_t.tv_usec;
	double tExecution = te - ts;
	std::cout << "FFD Execution time(us): " << tExecution << endl;
	out_file_alg1 << "FFD Execution time(us): " << tExecution << endl;
	out_file_alg1 << ffd1.Solution.power << "    " << ffd1.Solution.wastage << "    " << ffd1.Solution.numActivePMs << "    " << ffd1.Solution.utilCPU << "    " << ffd1.Solution.utilMEM << "    " << tExecution << endl;
	saveResultsofIteration(iterationForAvg, algIndex, ffd1.Solution.power, ffd1.Solution.wastage, ffd1.Solution.numActivePMs, ffd1.Solution.utilCPU, ffd1.Solution.utilMEM, tExecution);
}

void runWF(idata PMserv, unsigned int iterationForAvg, unsigned int algIndex)
{
	char name_of[100];
	ofstream out_file_alg1;
	string str_of1;
	string nvmstr = std::to_string(num_vm_experiment);
	string pselstr = std::to_string(EXPERIMENT_P_VALUE_SELECTOR);
	sprintf(name_of, "%s/RES_%s_%sVMs_%sPvalue.txt", FOLDER, ALG4, nvmstr.c_str(), pselstr.c_str());
	str_of1 = name_of;

	if (iterationForAvg == 0)
	{
		out_file_alg1.open(name_of, std::ofstream::out | std::ofstream::trunc);
		out_file_alg1.close();
	}
	out_file_alg1.open(str_of1, fstream::app);
	struct timeval st_t, end_t;
	std::cout << endl
			  << "==============WorstFit====================";

	gettimeofday(&st_t, NULL);
	WF wf1(PMserv.iPMs, PMserv.iVMs, Rp, Rm, P_array2[EXPERIMENT_P_VALUE_SELECTOR], out_file_alg1, 1, iterationForAvg);
	std::cout << endl
			  << "==============END WorstFit================" << endl;

	gettimeofday(&end_t, NULL);
	double ts = st_t.tv_sec * 1.0e6 + (double)st_t.tv_usec;
	double te = end_t.tv_sec * 1.0e6 + (double)end_t.tv_usec;
	double tExecution = te - ts;
	std::cout << "WF Execution time(us): " << tExecution << endl;
	out_file_alg1 << "WF Execution time(us): " << tExecution << endl;
	out_file_alg1 << wf1.Solution.power << "    " << wf1.Solution.wastage << "    " << wf1.Solution.numActivePMs << "    " << wf1.Solution.utilCPU << "    " << wf1.Solution.utilMEM << "    " << tExecution << endl;
	saveResultsofIteration(iterationForAvg, algIndex, wf1.Solution.power, wf1.Solution.wastage, wf1.Solution.numActivePMs, wf1.Solution.utilCPU, wf1.Solution.utilMEM, tExecution);
}

void runBF(idata PMserv, unsigned int iterationForAvg, unsigned int algIndex)
{
	char name_of[100];
	ofstream out_file_alg1;
	string str_of1;
	string nvmstr = std::to_string(num_vm_experiment);
	string pselstr = std::to_string(EXPERIMENT_P_VALUE_SELECTOR);
	sprintf(name_of, "%s/RES_%s_%sVMs_%sPvalue.txt", FOLDER, ALG3, nvmstr.c_str(), pselstr.c_str());
	str_of1 = name_of;

	if (iterationForAvg == 0)
	{
		out_file_alg1.open(name_of, std::ofstream::out | std::ofstream::trunc);
		out_file_alg1.close();
	}
	out_file_alg1.open(str_of1, fstream::app);
	struct timeval st_t, end_t;
	std::cout << endl
			  << "==============BestFit====================";

	gettimeofday(&st_t, NULL);
	BF bf1(PMserv.iPMs, PMserv.iVMs, Rp, Rm, P_array2[EXPERIMENT_P_VALUE_SELECTOR], out_file_alg1, 1, iterationForAvg);
	std::cout << endl
			  << "==============END BestFit================" << endl;

	gettimeofday(&end_t, NULL);
	double ts = st_t.tv_sec * 1.0e6 + (double)st_t.tv_usec;
	double te = end_t.tv_sec * 1.0e6 + (double)end_t.tv_usec;
	double tExecution = te - ts;
	std::cout << "BF Execution time(us): " << tExecution << endl;
	out_file_alg1 << "BF Execution time(us): " << tExecution << endl;
	out_file_alg1 << bf1.Solution.power << "    " << bf1.Solution.wastage << "    " << bf1.Solution.numActivePMs << "    " << bf1.Solution.utilCPU << "    " << bf1.Solution.utilMEM << "    " << tExecution << endl;
	saveResultsofIteration(iterationForAvg, algIndex, bf1.Solution.power, bf1.Solution.wastage, bf1.Solution.numActivePMs, bf1.Solution.utilCPU, bf1.Solution.utilMEM, tExecution);
}
int runHHO(idata PMserv, unsigned int iterationForAvg, unsigned int algIndex)
{
	char name_of[100];
	ofstream out_file_alg;
	string str_of1;
	string nvmstr = std::to_string(num_vm_experiment);
	string pselstr = std::to_string(EXPERIMENT_P_VALUE_SELECTOR);
	sprintf(name_of, "%s/RES_%s_%sVMs_%sPvalue.txt", FOLDER, ALG7, nvmstr.c_str(), pselstr.c_str());
	str_of1 = name_of;

	if (iterationForAvg == 0)
	{
		out_file_alg.open(name_of, std::ofstream::out | std::ofstream::trunc);
		out_file_alg.close();
	}
	out_file_alg.open(str_of1, fstream::app);
	struct timeval st_t, end_t;
	std::cout << endl
			  << "=======================HHO=======================" << endl;

	gettimeofday(&st_t, NULL);
	std::cout << " running HHO algorithm" << std::endl;

	Hho hho1( HHO_pop_size);
	hho1.initialize(PMserv.iVMs);
	if (VERBOSE)
		hho1.printPop();
	std::cout << "initialization of HHO pop is finished!" << std::endl;
	if (VERBOSE)
		PMserv.printPMsWithPower();
	hho1.run(HHO_epoch, PMserv.iVMs ,PMserv.iPMs);
	std::cout << endl
			  << "===================END HHO======================" << endl;

	gettimeofday(&end_t, NULL);
	double ts = st_t.tv_sec * 1.0e6 + (double)st_t.tv_usec;
	double te = end_t.tv_sec * 1.0e6 + (double)end_t.tv_usec;
	double tExecution = te - ts;
	std::cout << "HHO Execution time(us): " << tExecution << endl;
	out_file_alg << "HHO Execution time(us): " << tExecution << endl;
	out_file_alg << hho1.Solution.power << "    " << hho1.Solution.wastage << "    " << hho1.Solution.numActivePMs << "    " << hho1.Solution.utilCPU << "    " << hho1.Solution.utilMEM << "    " << tExecution << endl;
	/*resultsAllAlgorithmsAllIterations[algIndex][0][iterationForAvg]=hho1.Solution.power;
	resultsAllAlgorithmsAllIterations[algIndex][1][iterationForAvg]=hho1.Solution.wastage;
	resultsAllAlgorithmsAllIterations[algIndex][2][iterationForAvg]=hho1.Solution.numActivePMs;
	resultsAllAlgorithmsAllIterations[algIndex][3][iterationForAvg]=hho1.Solution.utilCPU;
	resultsAllAlgorithmsAllIterations[algIndex][4][iterationForAvg]=hho1.Solution.utilMEM;
	resultsAllAlgorithmsAllIterations[algIndex][5][iterationForAvg]=(float) tExecution;
	*/
	saveResultsofIteration(iterationForAvg, algIndex, hho1.Solution.power, hho1.Solution.wastage, hho1.Solution.numActivePMs, hho1.Solution.utilCPU, hho1.Solution.utilMEM, tExecution);

	return 0;
}

void runRF(idata PMserv, unsigned int iterationForAvg, unsigned int algIndex)
{
	char name_of[100];
	ofstream out_file_alg1;
	string str_of1;
	string nvmstr = std::to_string(num_vm_experiment);
	string pselstr = std::to_string(EXPERIMENT_P_VALUE_SELECTOR);
	sprintf(name_of, "%s/RES_%s_%sVMs_%sPvalue.txt", FOLDER, ALG5, nvmstr.c_str(), pselstr.c_str());
	str_of1 = name_of;

	if (iterationForAvg == 0)
	{
		out_file_alg1.open(name_of, std::ofstream::out | std::ofstream::trunc);
		out_file_alg1.close();
	}
	out_file_alg1.open(str_of1, fstream::app);
	struct timeval st_t, end_t;
	std::cout << endl
			  << "==============RandFit====================";

	gettimeofday(&st_t, NULL);
	RF rf1(PMserv.iPMs, PMserv.iVMs, Rp, Rm, P_array2[EXPERIMENT_P_VALUE_SELECTOR], out_file_alg1, 1, iterationForAvg);
	std::cout << endl
			  << "==============END RandFit================" << endl;

	gettimeofday(&end_t, NULL);
	double ts = st_t.tv_sec * 1.0e6 + (double)st_t.tv_usec;
	double te = end_t.tv_sec * 1.0e6 + (double)end_t.tv_usec;
	double tExecution = te - ts;
	std::cout << "RF Execution time(us): " << tExecution << endl;
	out_file_alg1 << "RF Execution time(us): " << tExecution << endl;
	out_file_alg1 << rf1.Solution.power << "    " << rf1.Solution.wastage << "    " << rf1.Solution.numActivePMs << "    " << rf1.Solution.utilCPU << "    " << rf1.Solution.utilMEM << "    " << tExecution << endl;
	saveResultsofIteration(iterationForAvg, algIndex, rf1.Solution.power, rf1.Solution.wastage, rf1.Solution.numActivePMs, rf1.Solution.utilCPU, rf1.Solution.utilMEM, tExecution);
}

int main()
{
	srand(time(NULL));
	unsigned int experimentID = EXPERIMENT_P_VALUE_SELECTOR; // to select different P values
	std::cout << endl
			  << "============================================START=============================================";
	std::cout << endl
			  << "P (in VM generator): " << P_array2[experimentID];
	for (unsigned int iter = 0; iter < NUM_ITERATIONS_FOR_AVG; iter++)
	{

		idata PMServ;
		PMServ.experimentID = EXPERIMENT_P_VALUE_SELECTOR;
		PMServ.createPMs(num_pm_experiment);
		// PMServ.createServices(num_services,MIN_VM_PER_SERVICE,MAX_VM_PER_SERVICE,experimentID);
		PMServ.printPMsWithPower();
		PMServ.createVMs(num_vm_experiment);
		PMServ.printVMs();

		// PMServ.printServices();
		// service theService;
		//  pm_list=thePM.experiment_pm(pm_list, n_exp,NUM_DC_PER_CLOUD,NUM_PM_PER_RACK,num_pm_types); // create VMs and PMs (with some proper resource capacities and requests)
		//  vm_list=experiment_vm(vm_list,n_exp);
		// pm::print_pm_set_with_type(pm_list);
		// service_list = service::experiment_service(service_list, n_exp,NUM_SERVICES,MIN_VM_PER_SERVICE,MAX_VM_PER_SERVICE,num_pm_experiment);
		// vm_list = collect_vms_into_a_list(service_list);

		// NOTE: last argument mush be consistant with alg neames:
		// algNames[7] = { " FFD "," RF ", " BF"," WF ", " GA ", " EVO " ," CRO "};

		runFFD(PMServ, iter, 0);
		runRF(PMServ, iter, 1);
		runBF(PMServ, iter, 2);
		runWF(PMServ, iter, 3);
		runGA(PMServ, iter, 4);
		printAllResultsInAFile();
		cout << "Simulation at iteration " << iter << " Ended OK" << endl;
	}
	return 0;
}
