/*
 * pm.cpp
 *
 *  Created on: Oct 2, 2023
 *      Author: mkiani
 */

#include "pm.h"

pm::pm()
{
}

pm::~pm()
{
}

void pm::set_utilization(PMutilization new_util)
{
	util = new_util;
}

void pm::initialize(bool init_state, int assigned_pm_id, int init_dc_id, int init_rack_id, int init_pm_type_index, PMresource init_res, int p_idle_pm, int p_max_pm)
{
	state = init_state;
	id = assigned_pm_id;
	dc = init_dc_id;
	rack = init_rack_id;
	type = init_pm_type_index;
	num_vms = 0;
	capacity = init_res;
	num_mig_time_in = 0;
	num_mig_time_out = 0;
	total_mig_in = 0;
	total_mig_out = 0;
	util.cpu = 0;
	util.mem = 0;
	util.bw = 0;
	p_max = p_max_pm;
	p_idle = p_idle_pm;
}
