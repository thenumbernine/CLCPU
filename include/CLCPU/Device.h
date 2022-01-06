#pragma once

#include "CLCPU/Dispatch.h"
#include <CL/cl.h>
#include <vector>

CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceInfo(cl_device_id    device,
                cl_device_info  param_name,
                size_t          param_value_size,
                void *          param_value,
                size_t *        param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceIDs(cl_platform_id   platform,
               cl_device_type   device_type,
               cl_uint          num_entries,
               cl_device_id *   devices,
               cl_uint *        num_devices) CL_API_SUFFIX__VERSION_1_0;

bool verifyDevice(const cl_device_id device);

extern std::vector<cl_device_id> allDevices;

extern int cl_device_verify;

struct _cl_device_id {
	struct _cl_icd_dispatch const * const dispatch = &dispatchTable;	
	
	int verify = cl_device_verify;
	
	//assign upon clCreatePlatform
	cl_platform_id platform = {};

	//should this be constructed upon first clCreateCommandQueue,
	// or should it be constructed upon device creation?
	//I'm doing the latter, but what do the specs say?
	_cl_command_queue* defaultCommandQueue = {};

// constants

	static constexpr cl_ulong maxMemAllocSize = 5461822664;
};
static_assert(offsetof(_cl_device_id, dispatch) == 0);
