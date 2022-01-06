#pragma once

#include "CLCPU/Dispatch.h"
#include <CL/cl.h>
#include <vector>

CL_API_ENTRY cl_mem CL_API_CALL
clCreateBuffer(cl_context   context,
               cl_mem_flags flags,
               size_t       size,
               void *       host_ptr,
               cl_int *     errcode_ret) CL_API_SUFFIX__VERSION_1_0;

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
	_cl_mem(
		size_t size_,
		cl_mem_flags flags_,
		uint8_t * host_ptr_,
		cl_context context_,
		cl_mem_object_type type_ = CL_MEM_OBJECT_BUFFER
	)
	:
		type(type_),
		flags(flags_),
		host_ptr(host_ptr_),
		context(context_),
		buffer(size_)
	{}
	
	struct _cl_icd_dispatch const * const dispatch = &dispatchTable;
	
	int verify = cl_mem_verify;
	
	cl_mem_object_type type = CL_MEM_OBJECT_ALLOCATION_FAILURE;
	cl_mem_flags flags = {};
	uint8_t * host_ptr = {};
	cl_context context = {};
	std::vector<uint8_t> buffer = {};		// the "opencl" side of the memory
};
static_assert(offsetof(_cl_mem, dispatch) == 0);

#define VERIFY_MEMORY_EXTRA_STRICT

bool verifyMem(_cl_mem* mem);
