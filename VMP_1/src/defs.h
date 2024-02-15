/*
 * definitions.h
 *
 *  Created on: Oct 2, 2023
 *      Author: mkiani
 */

#ifndef DEFS_H_
#define DEFS_H_

//=====================NETWORK==================
//==============================================
// net arch: fat-tree
// #core: d^2; #edge,aggregation: 2d^d; #pod:2d; #PMs:2d^3
#define num_port_sw 4 // d
#define num_layers_net 3
#define TOTAL_SW_L3 num_port_sw *num_port_sw
#define TOTAL_SW_L2 2 * num_port_sw *num_port_sw
#define TOTAL_SW_L1 2 * num_port_sw *num_port_sw
#define num_links_net 3 * (num_cor_sw + num_aggr_sw + num_edge_sw) // number of links in the netwrok of the DC

#define max_num_PMs 2 * num_port_sw *num_port_sw *num_port_sw;
const int l1_link_speed = 10000;  // 10gbps
const int l2_link_speed = 40000;  // 40gbps
const int l3_link_speed = 100000; // 100gbps
const int bandwidth_sw_per_layer[num_layers_net] = {2 * num_port_sw * num_port_sw * num_port_sw * l1_link_speed, 2 * num_port_sw *num_port_sw *num_port_sw *l2_link_speed, 2 * num_port_sw *num_port_sw *num_port_sw *l3_link_speed};
const float Q_max_layer[num_layers_net] = {9 * num_port_sw, 9 * num_port_sw, 36 * num_port_sw};
const float Q_idle_layer[num_layers_net] = {6.75 * num_port_sw, 27 * num_port_sw, 27 * num_port_sw};

// const int num_layers=log10(num_pm_experiment)*0.5;
#define NUM_PORT_PER_SW_EDGE 4
#define BW_EDGE_PORT_Gbps 10
#define BW_CORE_PORT_Gbps 40

#define INTER_PM_BW_SAME_RACK 1000 // Gbps
#define INTER_PM_BW_SAME_POD 1000  // Gbps
#define INTER_PM_BW_FAR 1000	   // Gbps

const int RACK_SIZE = (NUM_PORT_PER_SW_EDGE);
const int POD_SIZE = (NUM_PORT_PER_SW_EDGE * NUM_PORT_PER_SW_EDGE);
// static bool flay_first_t2=1;

enum Net_arch
{
	TREE,
	FATT,
	VL2
};

enum alg
{
	FFD,
	HEUR1,
	HEUR2
};
// Net_arch selected_net_arc=TREE;
static const int POWER_SWs[2][3] = { // first row: P_idle second row: P_max, columns: edge, aggr, core powers
	{75, 100, 200},
	{100, 150, 300}};

// VM net
// communication
#define MIN_COMM_BW 200									 // Mbps or MBs?
#define MAX_COMM_BW 400									 // Mbps 3000 5000
#define PERCENT_COMMUN_VMS 20							 // What percent (at most) of the VMs of the service have communications
#define PERCENT_PER_VM_COMMUNICATION 40					 // To what percent (at most) of VMs in the service a communicating VM has communication with?
#define MAX_SOUTN_NORTH_COMMUN_VOL (0.2 * (MAX_COMM_BW)) //
#define MIN_SOUTN_NORTH_COMMUN_VOL (0.2 * (MIN_COMM_BW))
#define percent_north_south_com_VMs 0.25 // 220 percent of total VMs have NS communications
// example: 10 VMs in the service, 2 communicating VMs (VM1 and VM2): VM1 has communication with 2 other VMs, VM2 with one other VM

const int num_pm_types = 1;
const int MAX_NUM_PM_TYPES = num_pm_types * 5;
#define MAX_NUM_PMS 200

static int PM_TYPES[MAX_NUM_PM_TYPES][6] = { // each row: a VM type. Four columns for CPU, MEM, inrackNET, inDCNET,interDCNET, disk
	{512, 4096, 20000, 10000},
	{512, 4096, 20000, 10000},
	{512, 4096, 20000, 10000},
	{512, 4096, 20000, 10000},
	{512, 4096, 20000, 10000}};
/*{128,2048,10000,10000},
 {256,2048,15000, 10000},
{512,4096,20000, 10000},
{1024,8192,30000, 100000},
{2048,16384,40000, 100000} */

// static int PM_MAP[MAX_NUM_PM_TYPES*100]={	//which type is each PM (#PM element each assigned with a number between 0 and %PM_TYPES-1)
//	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,0,1,2,3,4,0,1,2,3,4};

static const int POWER_PMs[2][MAX_NUM_PM_TYPES * 2] = { // first row: P_idle second ros: P_max
	//{75,100,100,150,200,200,250},
	//{100,150,175,200,250,300,400}
	{500, 100, 100, 150, 200, 200, 250},
	{800, 100, 100, 150, 200, 200, 250}};

#define INRACK_NET_SPEED 2000;
#define OUTRACK_NET_SPEED 1000;

// Cloud-Row-Rack-PM
#define NUM_DC_PER_CLOUD 1

// rack
#define NUM_RACK_DC 2
#define NUM_ROW_PER_DC 1
#define NUM_RACK_PER_ROW 2 * num_port_sw *num_port_sw
#define NUM_PM_PER_RACK num_port_sw
#define MAX_NUM_RACK_TYPES 5

// static int RACK_TYPES[MAX_NUM_RACK_TYPES][MAX_NUM_PM_TYPES];

#define NUM_SERVICES 200 // setting parameter 1

const int num_pm_experiment = NUM_SERVICES; // setting parameter 2         oled:NUM_DC_PER_CLOUD*NUM_PM_PER_RACK*(NUM_RACK_PER_ROW)*NUM_ROW_PER_DC;

#define NUM_ITERATIONS_FOR_AVG 10
#define EXPERIMENT_P_VALUE_SELECTOR 2 // setting parameter 3 to select one P value

const float P_array[5] = {0.00, 0.25, 0.50, 0.75, 1.0};
const float Rp = 0.45, Rm = 0.45; // setting parameter 4,5

const bool VERBOSE = 0;

#define MIN_VM_PER_SERVICE 1
#define MAX_VM_PER_SERVICE 1
#define num_vm_max NUM_SERVICES *MAX_VM_PER_SERVICE
const int num_services = NUM_SERVICES;

const int num_vm_experiment = num_services * MAX_VM_PER_SERVICE;

const float epsilon_wastage = 0.000001;

#define statr_index 1
#define NUM_EXPERIMENTS 2

const int NOT_A_PM_ID = (num_pm_experiment * 100);

//---------------=================---------EVO-related parameters
#define EVO_epoch 100
#define EVO_pop_size 100
#define EVO_num_dims num_vm_experiment

//---------------=================---------CRO-related parameters
#define CRO_epoch 150
#define CRO_pop_size 50
#define CRO_num_dims num_vm_experiment

#define max_FE_cro 7500

#define ALPHA_CRO 1
#define BETA_CRO 0.25
#define INITIAL_BUFFER 5
#define MOLLECULAR_COLLISION 0.6
#define KE_LOSS_RATE_CRO 0.2
#define INITIAL_KE 1

//---------------=================---------COA-related parameters
#define K_COA 1 + num_vm_experiment / 10;
#define COA_epoch 100
#define COA_pop_size 100
#define COA_num_dims num_vm_experiment
const float CROSSOVER_RATE_COA = 0.8;
const float MUTATION_RATE_COA = 0.1;
#define SOLOTUION_LENGTH_COA num_vm_experiment
#define MAX_ALLOWABLE_GENERATIONS_COA 400

// Number of individuals in each generation
#define max_FE_coa 7500
#define NULL_VALUE_COA num_vm_experiment * 10 // this is used to put into solution to show an unassigned index in a solution

//---------------=================---------GA-related parameters
#define GA_epoch 100
#define GA_pop_size 50
#define GA_num_dims num_vm_experiment
#define HHO_pop_size 100
#define HHO_epoch 200
const float CROSSOVER_RATE = 0.8;
const float MUTATION_RATE = 0.1;
#define MAX_ALLOWABLE_GENERATIONS 400

// Number of individuals in each generation
#define max_FE_ga 7500

#endif /* DEFS_H_ */
