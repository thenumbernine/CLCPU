#pragma once

#include "CLCPU/Dispatch.h"
#include <CL/cl.h>

CL_API_ENTRY cl_int CL_API_CALL
clRetainMemObject(cl_mem memobj) CL_API_SUFFIX__VERSION_1_0;

CL_API_ENTRY cl_int CL_API_CALL
clReleaseMemObject(cl_mem memobj) CL_API_SUFFIX__VERSION_1_0;

CL_API_ENTRY cl_int CL_API_CALL
clGetMemObjectInfo(cl_mem           memobj,
                   cl_mem_info      param_name,
                   size_t           param_value_size,
                   void *           param_value,
                   size_t *         param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

extern int cl_mem_verify;

struct _cl_mem {
	struct _cl_icd_dispatch const * const dispatch = &dispatchTable;
	
	int verify = cl_mem_verify;
	
	cl_mem_object_type type = {};
	size_t size = {};
	cl_mem_flags flags = {};
	uint8_t * ptr = {};
	uint8_t * hostPtr = {};
	cl_context context = {};
};
static_assert(offsetof(_cl_mem, dispatch) == 0);

#define VERIFY_MEMORY_EXTRA_STRICT

bool verifyMem(_cl_mem* mem);
